
// liste des séquence de commandes à envoyer à l'Arduino
let lcmd = []

// numéro de la séquence courante
let num = 0 

// port xbee
let xbee = null

let nom_mission = null
let distance_arret = null // distance d'arrêt

function init(xb,nom,vitesse,duree,distance) {
    console.log("init mission "+nom+" vitesse = "+vitesse+" durée = " + duree+" distance arrêt = "+distance)

    xbee = xb

    nom_mission = nom
    distance_arret = distance

    lcmd = [`[[mda ${vitesse}][mga ${vitesse}][t ${duree}]]`]

    num = 0
}

function exec(etat) {
    console.log("exec mission "+nom_mission+" etat = "+JSON.stringify(etat))

    if (xbee != null) {
        if (etat.d1 < distance_arret) {
            console.log("fin de la mission")
            num++
        }
        else {
            console.log("envoi séquence "+lcmd[num])
            xbee.write(lcmd[num])
        }
    }
}

module.exports = {
    init : init,
    exec : exec,
}

