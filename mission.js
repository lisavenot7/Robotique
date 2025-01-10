//const { exec } = require("child_process")


// tableau des missions
let lmissions = [
    {
        nom : "avancer",
        module : require("./mission_00_avancer.js"),
    },
    {
        nom : "avancer-tourner",
        module : require("./mission_01_avancer_tourner.js"),
    },
    {
        nom : "avancer-obstacle",
        module : require("./mission_02_avancer_obstacle.js"),
    },
    {
        nom : "longer",
        module : require("./mission_03_longer.js"),
    },
    {
        nom : "boite",
        module : require("./mission_04_longer_boite.js"),
    },
]


let mission = null // mission courante

function chercher_mission(nom) {
    mission = null
    for (let i=0 ; i<lmissions.length ; i++) {
        if (lmissions[i].nom == nom) {
            mission = lmissions[i]
            return
        }
    }
    console.log("erreur mission.js function chercher_mission : non trouvé "+nom)
}

function init(xbee,nom,val1,val2,val3) {

    chercher_mission(nom)

    if (mission == null) {
        console.log("erreur mission.js function init : mission = null")
    }
    else {
        mission.module.init(xbee,nom,val1,val2,val3)
    }

    // pour démarrer la mission il faut envoyer une commande au robot
    // quand le robot va répondre, ça va déclencher l'exécution de la fonction "exec" de la mission
    xbee.write("[[ t 100]]")

}

function exec(etat) {
 
    if (mission == null) {
        console.log("erreur mission.js function exec : mission = null")
    }
    else {
        mission.module.exec(etat)
    }

}

module.exports = {
    init : init,
    exec : exec,
}



