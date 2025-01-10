const { SerialPort } = require("serialport")

const mission = require("./mission.js")

//const tty = "COMx"  // sur Windows remplacer COMx par le nom d'un port COM (COM3 ou COM4 ou ...)
//const tty = "/dev/ttyACM0"  // nom du port USB XBee sur Linux
//const tty = "/dev/ttyUSB0"  // nom du port USB Arduino sur Linux

const tty = "/dev/ttyACM0" // à modifier sur Windows (le logiciel Arduino indique le port)
// la détection automatique du port est également possible en nodejs

const baudRate = 9600 // vitesse des échanges

var xbee = null
var xbee_ok = false // les E/S sont possibles si xbee_ok == true

var chunk = "" // pour concatener les réponses de l'Arduino qui arrivent par morceaux

var etat = null // etat du robot (distances mesurées)

var messages = [] // messages pour l'IHM


function usb_fermer() {
    if (xbee != null) {
        xbee.close()
        xbee = null
        xbee_ok = false
    }
    let m = 'Port USB fermé : ' + tty
    console.log(m);
    messages.push({id : "usb", mess : m})
}

function usb_ouvrir() {
    console.log("usb.js : exec init")

    if (xbee != null) {
        messages.push({id : "usb", mess : "Port " + tty + " déjà ouvert"})
        return
    }

    xbee = new SerialPort({ // fonctionne pour USB et XBee
        path: tty,
        baudRate: baudRate,
    });

    xbee.on('open', function () {
        let m = 'Port USB ouvert : ' + tty
        console.log(m);
        xbee_ok = true

        messages.push({id : "usb", mess : m})

        //mission.init(xbee)
        
        //xbee.write("[[t 1000]]") 
        // on amorce le processus
        // l'Arduino reçoit une commande et envoie une réponse ce qui déclenche l'envoi d'une nouvelle commande
    

    });

    xbee.on('error', function (err) {
        let m = "Erreur usb.js : ouverture port " + tty + " (port incorrect ou déjà ouvert)"
        console.log(m)
        messages.push({id : "usb", mess : m})

        console.log("La variable tty doit contenir le nom du port")
        console.log("/dev/ttyACM0 sous Linux")
        console.log("COMx sous Windows où x est un numéro (COM3, COM4, ...)")
        console.log("Sous Windows, utiliser le logiciel Arduino pour trouver le port COM")
        console.log("Clic sur Outils / Port pour voir les ports utilisés")
        //console.log("Sous Windows taper chgport dans une invite de commande pour voir les ports COM utilisés, ou bien utiliser le logiciel Arduino pour voir le port COM utilisé")

        //process.exit(0)
    });

    xbee.on('data', function (data) {
        chunk += data.toString()
        //console.log("type = " + typeof (data))
        //console.log("chunk = " + chunk)
        for (; ;) {
            var pos = chunk.indexOf("\n")
            if (pos < 0) break;
            //console.log(usb.chunk.substring(0,pos));

            let ln = chunk.substring(0, pos)
            chunk = chunk.substring(pos + 1)

            if (ln.startsWith("{")) { // on a une réponse JSON (état de l'Arduino)
                etat = JSON.parse(ln)
                //console.log("Réponse Arduino json = "+JSON.stringify(etat))
                mission.exec(etat)
            }
            else {
                console.log("Réponse Arduino txt = "+ln)
            }
        }
    })
}

function check() { // retourne l'état du robot et les messages pour l'IHM

    if (messages.length > 0) {
        console.log("robot.check : nb messages = "+messages.length)
    }

    let m = messages
    messages = []

    return {
        etat : etat,
        messages : m,
    }
}

function init_mission(nom,val1,val2,val3) {
    mission.init(xbee,nom,val1,val2,val3)
}

//init()

// on attend 2 secondes avant d'exécuter une mission
// setTimeout(() => {
//     mission.init(xbee)
// }, 2000);

// setTimeout(() => {
//     xbee.write("[[mda 255][mga 255][t 500]]")
// }, 2000);

module.exports = {
    usb_ouvrir : usb_ouvrir,
    usb_fermer : usb_fermer,
    check : check,
    init_mission : init_mission,
}


let num = 0

if (num == 1) { // lancement de la mission "avancer"
    usb_ouvrir()

    // on attend 1 seconde que le port USB/XBEE soit ouvert
    setTimeout(() => {
        // lancement de la mission "avancer" avec les paramètres vitesse à 255 et durée à 1000
        init_mission("avancer","255","1000",null)
    }, 1000)
    
    // on attend 10 secondes avant de fermer le port USB/XBEE
    setTimeout(() => {
        usb_fermer()
    }, 10000)
}

if (num == 2) { // lancement de la mission "avancer-obstacle"
    usb_ouvrir()

    // on attend 1 seconde que le port USB/XBEE soit ouvert
    setTimeout(() => {
        // lancement de la mission "avancer" avec les paramètres vitesse à 255 et durée à 1000
        init_mission("avancer-obstacle","255","1000",null)
    }, 1000)
    
}

if (num == 3) { // lancement de la mission "avancer-obstacle"
    usb_ouvrir()

    // on attend 1 seconde que le port USB/XBEE soit ouvert
    setTimeout(() => {
        // lancement de la mission "avancer" avec les paramètres vitesse à 255 et durée à 1000
        init_mission("longer","255","1000",null)
    }, 1000)
    
}

if (num == 4) { // lancement de la mission "longer-boite"
    usb_ouvrir()

    // on attend 1 seconde que le port USB/XBEE soit ouvert
    setTimeout(() => {
        // lancement de la mission "avancer" avec les paramètres vitesse à 255 et durée à 1000
        init_mission("boite","255","1000",null)
    }, 1000)
    
}



