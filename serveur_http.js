
const express = require('express')
const fs = require('fs')

const robot = require("./robot.js")

var base = "."
var port_http = 3200

if (!fs.existsSync(base)) {
	console.log("Erreur : le dossier " + base + " n'existe pas")
	console.log("Modifier la variable base dans serveur_http_v1.js")
	console.log("La variable base doit contenir un chemin absolu ")
	console.log("Le programme a été arrêté ")
	process.exit(0)
}

/***************************************************************************************************/
/******************************************** serveur HTTP *****************************************/
/***************************************************************************************************/

var app = express();
app.use("/", express.static(base + "/html", { index: 'index.html' }))
app.use(express.json({ limit: '50mb' }))
app.use(express.urlencoded({ extended: true }))

var server = app.listen(port_http, function () {
	console.log('Express server listening on port ' + port_http)
});

// test
app.get("/test", function (req, res) {
	console.log("GET test")
	res.send({ mess: "Réponse du serveur : requête test reçue" })
})

app.post("/usb_ouvrir", function (req, res) {
	console.log("POST /usb_ouvrir")
	robot.usb_ouvrir()
})

app.post("/usb_fermer", function (req, res) {
	console.log("POST /usb_fermer")
	robot.usb_fermer()
})

app.post("/mission", function (req, res) {
	//console.log(util.inspect(req.body))
	console.log("POST mission nom = " + req.body.nom + " val1  = " + req.body.val1 + " val2 = " + req.body.val2 + " val3 = " + req.body.val3)

	robot.init_mission(req.body.nom, req.body.val1, req.body.val2, req.body.val3)


	res.send({ mess: "Commande envoyée au robot : " + req.body.nom })
})

app.post("/check", function (req, res) { // recuperation de l'etat du robot et des messages

	//console.log("POST /check")
	let r = robot.check()

	//console.log("check = "+JSON.stringify(r))

	res.send(r) // on retourne l'état de l'Arduino et les messages pour l'IHM



})






