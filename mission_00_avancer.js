
// liste des séquence de commandes à envoyer à l'Arduino

let lcmd = []

// numéro de la séquence courante
let num = 0 

// port xbee
let xbee = null

let nom_mission = null

function init(xb,nom,vitesse,duree) {
    console.log("init mission "+nom+" vitesse = "+vitesse+" durée = " + duree)

    xbee = xb

    nom_mission = nom

    lcmd = [`[[mda ${vitesse}][mga ${vitesse}][t ${duree}]]`]
    num = 0
}

function exec(etat) {
    console.log("exec mission "+nom_mission+" etat = "+JSON.stringify(etat))
      if (xbee != null) {
        if (num < lcmd.length) {
            console.log("envoi séquence "+lcmd[num])
            xbee.write(lcmd[num]) // envoi de la séquence courante
            num++
        }
        else {
            console.log("fin de la mission")
        }
    }
}

module.exports = {
    init : init,
    exec : exec,
}

