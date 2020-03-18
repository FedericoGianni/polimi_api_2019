Progetto di Algoritmi e Strutture Dati - 2019

Si vuole implementare un meccanismo di monitoraggio di relazioni tra
entità (per esempio persone) che cambiano nel tempo

• Si immagini, per esempio, un social network, in cui nuovi utenti
possono registrarsi, e utenti esistenti possono cancellare il proprio
account, diventare “amici” di altri utenti, rompere la relazione di
amicizia, ecc.

• Le relazioni tra entità non sono necessariamente simmetriche. Per
esempio, Alice può essere “amica” di Bruno, ma l’amicizia non è
reciprocata (Bruno non è amico di Alice)

In maniera più astratta, il meccanismo monitora i seguenti fenomeni:

• Una nuova entità comincia ad essere monitorata
• Una entità monitorata smette di esserlo
• Una nuova relazione viene stabilita tra 2 entità monitorate
• Una relazione esistente tra 2 entità monitorate cessa di esistere
• Ogni entità ha un nome identificativo (per esempio "Alice", "Bruno", "Carlo")
• Ci possono essere diversi tipi di relazioni tra entità, ognuna identificata da un nome (per esempio,
"amico_di", "segue", "coetaneo_di")
• Ogni relazione ha un verso (per esempio, se Alice è "amico_di" Bruno, il verso della relazione è da
Alice a Bruno, quindi Bruno è il “ricevente” della relazione), e non è necessariamente simmetrica
• A seguito di un apposito comando, il sistema restituisce, per ogni relazione, l’entità che “riceve”
più relazioni (se ci sono più entità il cui numero di relazioni ricevute è massimo, queste vengono
stampate in ordine crescente di identificativo)
• L’applicativo dovrà essere ottimizzato per gestire un grande numero di entità e istanze di relazioni,
ma generalmente pochi tipi (identificativi) di relazione

Implementazione in linguaggio C standard (con la sola libc) di un
programma che legge da standard input una sequenza di comandi,
ognuno corrispondente ad un cambiamento nelle entità o nelle
relazioni tra entità e, quando richiesto, produce su standard output,
per ogni tipo di relazione monitorata, l'identificativo dell'entità che è
il ricevente del maggior numero di istanze di quella relazione, e il
numero di relazioni che l'entità riceve
