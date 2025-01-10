let lcmd = [
    "avancer",
    "tourner",
    ]

let num_cmd = 0

// mise à jour dist1 et dist2

if (lcmd[num_cmd] == "avancer") {
    // avancer tout droit pendant n ms
   }
   else if (lcmd[num_cmd] == "tourner") {
    if ((dist1 != null) && (dist1 != null)) {
     if (dist1 < dist2) {
      // tourner dans un sens pendant m1 ms
     }
     else {
      // tourner dans l’autre sens pendant m2 ms
     }
    }
    else {
     send_cmd("[[t 100]]")
    } 
   }
   num_cmd++
   if (num_cmd >= lcmd.length) num_cmd = 0