const char html[] PROGMEM =R"rawliteral('
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <title>Maaau - mi gladni!</title>
    <link href="https://maxcdn.bootstrapcdn.com/bootswatch/4.1.1/superhero/bootstrap.min.css" rel="stylesheet">
    <style type="text/css">
        .status {border: 1px dashed red;}
    </style>
    <script src="https://code.jquery.com/jquery-3.6.0.slim.min.js" integrity="sha256-u7e5khyithlIdTpu22PHhENmPcRdFiHRjhAuHcs05RI=" crossorigin="anonymous"></script>
    <script>
        function toggleMotor(element) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/run?feed="+element.value+"", true);
            xhr.send();
        }


        function on_load() {
            switch (document.getElementById("timer-state").innerText) {
                case 1:
                    document.getElementById("timer-state").innerHTML = "1 - ugašeno";
                    break;
                case 2:
                    document.getElementById("timer-state").innerHTML = "2 - svakih 5 sekundi";
                    break;
                case 3:
                    document.getElementById("timer-state").innerHTML = "3 - svaka 2 sata";
                    break;
                case 4:
                    document.getElementById("timer-state").innerHTML = "4 - svaka 3 sata";
                    break;
                case 5:
                    document.getElementById("timer-state").innerHTML = "5 - svaka 4 sata";
                    break;
            };
            switch (document.getElementById("manual-state").innerText) {
                case 1:
                    document.getElementById("manual-state").innerHTML = "malo";
                    break;
                case 2:
                    document.getElementById("manual-state").innerHTML = "malo više";
                    break;
                case 3:
                    document.getElementById("manual-state").innerHTML = "puno";
                    break;
            };
        };

        function change_state() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 1 && this.status == 200) {
                    switch (this.responseText.charCodeAt(0)) {
                        case 1:
                            document.getElementById("timer-state").innerHTML = "1 - ugašeno";
                            break;
                        case 2:
                            document.getElementById("timer-state").innerHTML = "2 - svakih 5 sekundi";
                            break;
                        case 3:
                            document.getElementById("timer-state").innerHTML = "3 - svaka 2 sata";
                            break;
                        case 4:
                            document.getElementById("timer-state").innerHTML = "4 - svaka 3 sata";
                            break;
                        case 5:
                            document.getElementById("timer-state").innerHTML = "5 - svaka 4 sata";
                            break;
                    };
                }
            };
            xhttp.open("GET", "/change_timer?p=" + document.getElementById("new_timer").value, true);
            xhttp.send();
        };

    </script>

  </head>
  <body onload="on_load()">
    <div class="container">

        <div class="row">
            <div class="col-sm-12 mb-5">
                <h1>Maaau - mi gladni!</h1>
            </div>

            <div class="col-sm-12">
                <h4>Daj hrane</h4>
            </div>
            <!-- <div class="col-sm-12">
                <div class="alert alert-secondary">
                    <h6 class="alert-heading pt-1">Manualno izbacujem: <span id="manual-state">%MANUAL_STATE%</span></h6>
                </div>
            </div> -->
            <div class="col-sm-4 text-center pb-3">
                <button type="button" value="5" onclick="toggleMotor(this)" class="btn btn-success btn-lg">Malo</button>
            </div>
            <div class="col-sm-4 text-center pb-3">
                <button type="button" value="10" onclick="toggleMotor(this)" class="btn btn-warning btn-lg">Malo vise</button>
            </div>
            <div class="col-sm-4 text-center pb-3">
                <button type="button" value="15" onclick="toggleMotor(this)" class="btn btn-danger btn-lg">Puno</button>
            </div>
        </div>
<hr />
        <div class="row mt-5">
            <div class="col-sm-6">
                <h4>Odaberi interval automatskog hranjenja:</h4>
                <p class="text-secondary">Trenutno je postavljeno svakih: <span class="text-warning" id="timer-state">%TIMER_STATE%</span></p>
            </div>
            <div class="col-md-6 text-center">
                <div class="form-group row text-center">
                    <select class="form-select" id="new_timer">
                        <option value="1">1 - ugašeno</option>
                        <option value="2">2 - svakih 5 sekundi</option>
                        <option value="3">3 - svaka 2 sata</option>
                        <option value="4">4 - svaka 3 sata</option>
                        <option value="5">5 - svaka 4 sata</option>
                    </select>
                    <button class="btn btn-secondary btn-lg" id="lock_command_button" onclick="change_state()">Promijeni</button>
                </div>
            </div>
        </div>

        <footer class="text-center mt-5">&copy; <a href="https://wisemedia.hr/" target="_blank">WiseMedia.hr</a> - Mau Maau Maaaaaaaau - daj hrane!</footer>
    </div>


  </body>
</html>
')rawliteral";