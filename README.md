# HTTP-CLIENT
Acest proiect este o aplicație client-server ce permite utilizatorilor 
să se înregistreze, să se autentifice, să acceseze o bibliotecă virtuală, 
să vizualizeze cărțile disponibile, să adauge cărți noi și să ștergă cărți 
existente. Aplicația este construită folosind limbajul C și include mai 
multe fișiere sursă și de antet pentru gestionarea funcționalităților 
aplicației.

Descriere fisiere
buffer.c
Acest fisier contine implementarea unor functii pentru manipularea unui buffer. 
Acesta este folosit pentru a gestiona datele citite de la socket, permitand 
adaugarea, gasirea si eliminarea continutului intr-un mod eficient.

buffer.h
Acest fisier contine declaratiile functiilor si structurilor utilizate in 
buffer.c. Este header-ul asociat fisierului buffer.c si trebuie inclus acolo 
unde se folosesc functiile de buffer.

client.c
Acest fisier contine implementarea functiei main si logica principala a 
clientului care interactioneaza cu serverul. Aici sunt definite comenzile 
de inregistrare, autentificare, accesare biblioteca, vizualizare carti, 
adaugare si stergere carti, precum si logout.

helpers.c
Acest fisier contine functii auxiliare pentru gestionarea conexiunilor 
de retea si a mesajelor HTTP. Include functii pentru deschiderea si 
inchiderea conexiunilor, trimiterea si primirea mesajelor de la server 
si extragerea raspunsurilor JSON.

helpers.h
Acest fisier contine declaratiile functiilor si structurilor utilizate 
in helpers.c. Este header-ul asociat fisierului helpers.c si trebuie 
inclus acolo unde se folosesc functiile de ajutor.

parson.c
Acest fisier contine implementarea bibliotecii Parson pentru manipularea 
JSON-urilor. Biblioteca Parson este utilizata pentru parsarea, crearea 
si manipularea obiectelor JSON, fiind esentiala pentru comunicarea 
client-server in format JSON.

parson.h
Acest fisier contine declaratiile functiilor si structurilor utilizate 
in parson.c. Este header-ul asociat bibliotecii Parson si trebuie inclus 
acolo unde se folosesc functiile de manipulare a JSON-urilor.

requests.c
Acest fisier contine implementarea functiilor pentru construirea 
cererilor HTTP (GET, POST, DELETE). Include functii pentru generarea 
mesajelor HTTP in format text, adaugarea header-elor, a cookie-urilor 
si a token-urilor de autentificare.

requests.h
Acest fisier contine declaratiile functiilor si structurilor utilizate 
in requests.c. Este header-ul asociat fisierului requests.c si trebuie 
inclus acolo unde se folosesc functiile pentru construirea cererilor HTTP.

Sleep days: 2