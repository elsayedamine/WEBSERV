const fs = require("fs");
const path = require("path");

const queryString = process.env.QUERY_STRING || "";
const queryParams = new URLSearchParams(queryString);
const queryUser = queryParams.get("user");
const queryMode = queryParams.get("mode");
const queryCount = queryParams.get("count");

function serializeCookie(name, value, options) {
	let cookieString = `${name}=${value}`;
	if (options && options.maxAge != null) cookieString += `; Max-Age=${options.maxAge}`;
	if (options && options.path) cookieString += `; Path=${options.path}`;
	if (options && options.httpOnly) cookieString += "; HttpOnly";
	if (options && options.secure) cookieString += "; Secure";
	if (options && options.sameSite) cookieString += `; SameSite=${options.sameSite}`;
	return cookieString;
}

function writeRedirect(statusLine, location, setCookieHeaders) {
	const body = "Redirecting...\n";
	let headers = "";
	headers += `Status: ${statusLine}\r\n`;
	headers += `Location: ${location}\r\n`;
	if (setCookieHeaders) {
		const headerList = Array.isArray(setCookieHeaders) ? setCookieHeaders : [setCookieHeaders];
		for (let i = 0; i < headerList.length; i++) {
			if (!headerList[i]) continue;
			headers += `Set-Cookie: ${headerList[i]}\r\n`;
		}
	}
	headers += "Content-Type: text/plain; charset=utf-8\r\n";
	headers += `Content-Length: ${Buffer.byteLength(body, "utf8")}\r\n`;
	headers += "\r\n";
	process.stdout.write(headers + body);
}

function parseCookies(cookieHeader) {
	const cookies = {};
	if (!cookieHeader) return cookies;
	const parts = cookieHeader.split(";");
	for (let i = 0; i < parts.length; i++) {
		const part = parts[i].trim();
		if (!part) continue;
		const eq = part.indexOf("=");
		if (eq === -1) continue;
		const key = part.slice(0, eq).trim();
		const value = part.slice(eq + 1).trim();
		if (key) cookies[key] = value;
	}
	return cookies;
}

function safeDecodeURIComponent(value) {
	if (value == null) return "";
	try {
		return decodeURIComponent(String(value));
	} catch (e) {
		return String(value);
	}
}

function decodeSessionCookieValue(rawValue) {
	if (!rawValue) return null;
	let decoded = rawValue;
	try {
		decoded = decodeURIComponent(rawValue);
	} catch (e) {
		// keep raw value
	}
	try {
		const parsed = JSON.parse(decoded);
		if (!parsed || typeof parsed !== "object") return null;
		return {
			user: parsed.user != null ? String(parsed.user) : "",
			count: parsed.count != null ? String(parsed.count) : "",
		};
	} catch (e) {
		return null;
	}
}

if (queryMode === "set") {
	if (!queryUser) {
		const body = "Missing 'user' in query string\n";
		process.stdout.write(
			"Status: 400 Bad Request\r\n" +
			"Content-Type: text/plain; charset=utf-8\r\n" +
			`Content-Length: ${Buffer.byteLength(body, "utf8")}\r\n` +
			"\r\n" +
			body
		);
		process.exit(0);
	}

	const encodedUser = encodeURIComponent(String(queryUser));
	const encodedCount = encodeURIComponent(String(queryCount != null ? queryCount : "0"));
	const sessionToken = `${Date.now().toString(36)}.${Math.random().toString(36).slice(2)}`;
	const setSessionCookie = serializeCookie("session", sessionToken, {
		path: "/",
		httpOnly: true,
		sameSite: "Lax",
	});
	const setUserCookie = serializeCookie("user", encodedUser, {
		path: "/",
		sameSite: "Lax",
	});
	const setCountCookie = serializeCookie("count", encodedCount, {
		path: "/",
		sameSite: "Lax",
	});
	writeRedirect("302 Found", "/cgi-bin/cookie_manager.js", [setSessionCookie, setUserCookie, setCountCookie]);
	process.exit(0);
}

if (queryMode === "delete") {
	const deleteSessionCookie = serializeCookie("session", "", {
		path: "/",
		maxAge: 0,
		httpOnly: true,
		sameSite: "Lax",
	});
	const deleteUserCookie = serializeCookie("user", "", {
		path: "/",
		maxAge: 0,
		sameSite: "Lax",
	});
	const deleteCountCookie = serializeCookie("count", "", {
		path: "/",
		maxAge: 0,
		sameSite: "Lax",
	});
	writeRedirect("302 Found", "/cgi-bin/cookie_manager.js", [deleteSessionCookie, deleteUserCookie, deleteCountCookie]);
	process.exit(0);
}


const cookies = parseCookies(process.env.HTTP_COOKIE);

// Backward-compat: older version stored JSON in the `session` cookie.
const legacySession = decodeSessionCookieValue(cookies.session);
const hasSession = Boolean(cookies.session);
const userFromCookie = cookies.user ? safeDecodeURIComponent(cookies.user) : (legacySession && legacySession.user ? legacySession.user : "");
const countFromCookie = cookies.count ? safeDecodeURIComponent(cookies.count) : (legacySession && legacySession.count ? legacySession.count : "0");

if (!hasSession || !userFromCookie) {
	const filePath = path.join(__dirname, "../login/index.html");

	try {
		const body = fs.readFileSync(filePath, "utf8");
		process.stdout.write(
			"Status: 200 OK\r\n" +
			"Content-Type: text/html; charset=utf-8\r\n" +
			`Content-Length: ${Buffer.byteLength(body, "utf8")}\r\n` +
			"\r\n" +
			body
		);
	} catch (err) {
		process.stdout.write(
			"Status: 500 Internal Server Error\r\n" +
			"Content-Type: text/html; charset=utf-8\r\n" +
			"\r\n"
		);
	}
	process.exit(0);
}

const filePath = path.join(__dirname, "../tests/index.html");
try {
	let body = fs.readFileSync(filePath, "utf8");
	body = body.replace(/\{\{USER\}\}/g, userFromCookie);
	body = body.replace(/\{\{COUNT\}\}/g, countFromCookie);
	process.stdout.write(
		"Status: 200 OK\r\n" +
		"Content-Type: text/html; charset=utf-8\r\n" +
		`Content-Length: ${Buffer.byteLength(body, "utf8")}\r\n` +
		"\r\n" +
		body
	);
} catch (err) {
	process.stdout.write(
		"Status: 500 Internal Server Error\r\n" +
		"Content-Type: text/plain; charset=utf-8\r\n" +
		"\r\n" +
		"Failed to load index\n"
	);
}

process.exit(0);
