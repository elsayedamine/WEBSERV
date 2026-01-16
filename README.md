amine: 
	i need to validate the host/port duplicates btwn servers

	if the config file is like : listen 80; the host is by default 0.0.0.0 !!

	also need to seperate with the newlines and not ignore them

	use the threads to parse the config file faster (ha 3ya9a)
				^
				|
				|
		a gls lard galak
		wa chof l dik jiha galak
		*seriously la rah parsing is already fast enough we don't need threads it's just a waste of time*

	don't forget to handle comments

	chof m3ak client_max_body_size rah makhdamach

	server_name can have any value, for example 127.0.0.1:8080 or localhost:8080

	redirect should be renamed to return (or ret lamakhlawkch dir return) and the second value can be empty

----------- NEW TO DO LIST -----------
amine:
	low priority:
	- double check the old to do list first
	high priority:
	- PUT and DELETE requests
	- handle CGI

youssef:
	high priorty:
	- session management

anyone:
- reorganize codebase