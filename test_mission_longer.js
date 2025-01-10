const robot = require("./robot.js")

robot.usb_ouvrir()

// on attend 1 seconde que le port USB/XBEE soit ouvert
setTimeout(() => {
  // lancement de la mission "longer" sans paramètres (paramètres à null)
  robot.init_mission("longer",null,null,null)
}, 1000)