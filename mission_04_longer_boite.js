const BOITE_LONGER = 0
const BOITE_PAS = 1
const BOITE_TOURNER = 2
const BOITE_TROUVER = 3

let mode = null

// port xbee
let xbee = null

let nom_mission = null


// sauvegarde valeurs variables
let dist = null
let v = null
let dt = null

function init(xb,nom,vitesse,duree,distance) {
    console.log("init mission "+nom+" vitesse = "+vitesse+" durée = " + duree+" distance arrêt = "+distance)

    xbee = xb

    nom_mission = nom

    dist = distance
    v = vitesse
    dt = duree

    mode = BOITE_LONGER
}

function exec(etat) {
    if (xbee != null) {
        if (mode==BOITE_LONGER) {
            console.log("boîte longer");
            if (etat.r1>1000) {
                xbee.write("[[mda 180][mga 180][t 500]]");
            }
            else {
                mode = BOITE_PAS;
                xbee.write("[[t 100]]");
            }
        }
        else if (mode == BOITE_PAS) {
            console.log("boîte pas");
            xbee.write("[[mda 180][mga 180][t 500]]");
            mode = BOITE_TOURNER;
        }
        else if (mode == BOITE_TOURNER) {
            console.log("boîte tourner");
            xbee.write("[[mdr 255][mga 255][t 500]]");
            mode = BOITE_TROUVER;
        }
        else if (mode == BOITE_TROUVER) {
            console.log("boîte trouver");
            if (etat.r1>1000) {
                mode = BOITE_LONGER;
                xbee.write("[[t 100]]");
            }
            else {
                xbee.write("[[mda 180][mga 180][t 500]]");
            }
        }
        else {
            console.log("erreur longer boîte : "+mode);
        }
    }
    
    console.log("exec mission "+nom_mission+" etat = "+JSON.stringify(etat));
}

module.exports = {
    init : init,
    exec : exec,
}
