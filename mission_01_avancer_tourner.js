
// liste des séquence de commandes à envoyer à l'Arduino
let lcmd = [
    "[[mda 255][mga 255][t 500]]",
    "[[mda 255][mgr 255][t 200]]",
]

// numéro de la séquence courante
let num = 0 

// port xbee
let xbee = null


function init(xb) {
    xbee = xb
}

function exec(etat) {
    if (xbee != null) {
        if (num < lcmd.length) {
            console.log("cmd = "+lcmd[num])
            xbee.write(lcmd[num]) // envoi de la séquence courante
            num++
        }
    }
}

module.exports = {
    init : init,
    exec : exec,
}

