const fs = require("fs");
const path = require("path");

const queryString = process.env.QUERY_STRING || "";
const queryParams = new URLSearchParams(queryString);
const queryUser = queryParams.get("user");
const queryMode = queryParams.get("mode");

function serializeCookie(name, value, options) {
	let cookieString = `${name}=${value}`;
	if (options && options.maxAge != null) cookieString += `; Max-Age=${options.maxAge}`;
	if (options && options.path) cookieString += `; Path=${options.path}`;
	if (options && options.httpOnly) cookieString += "; HttpOnly";
	if (options && options.secure) cookieString += "; Secure";
	if (options && options.sameSite) cookieString += `; SameSite=${options.sameSite}`;
	return cookieString;
}

function writeRedirect(statusLine, location, setCookieHeader) {
	const body = "Redirecting...\n";
	let headers = "";
	headers += `Status: ${statusLine}\r\n`;
	headers += `Location: ${location}\r\n`;
	if (setCookieHeader) headers += `Set-Cookie: ${setCookieHeader}\r\n`;
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

	const cookieValue = encodeURIComponent(queryUser);
	const setCookie = serializeCookie("user", cookieValue, {
		path: "/",
		httpOnly: true,
		sameSite: "Lax",
	});

	const selfPath = "/cgi-bin/cookie_manager.js";
	writeRedirect("301 Moved Permanently", selfPath, setCookie);
	process.exit(0);
}

if (queryMode === "delete") {
	const deleteCookie = serializeCookie("user", "", {
		path: "/",
		maxAge: 0,
		httpOnly: true,
		sameSite: "Lax",
	});

	const selfPath = "/cgi-bin/cookie_manager.js";
	writeRedirect("301 Moved Permanently", selfPath, deleteCookie);
	process.exit(0);
}


if (!process.env.HTTP_COOKIE) {
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

const cookies = parseCookies(process.env.HTTP_COOKIE);
let userFromCookie = cookies.user || "";
try {
	userFromCookie = decodeURIComponent(userFromCookie);
} catch (e) {
	// keep raw value
}

const filePath = path.join(__dirname, "../tests/index.html");
try {
	let body = fs.readFileSync(filePath, "utf8");
	body = body.replace(/\{\{USER\}\}/g, userFromCookie);
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
