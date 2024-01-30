<?php
    $PASSWORD = "";
    $INFOFILE = "infofile";

    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        $data = json_decode(file_get_contents('php://input'));
        // 4 newlines.
        if (strlen($data->infotext) > 4) {
            if ($data->password === $PASSWORD) {
                $file = fopen($INFOFILE,"w");
                // Max length 4*16 chars + 4 newlines.
                fwrite($file, substr($data->infotext, 0, 68));
                fclose($file);
                echo "OK";
                return;
            } else {
                echo "Wrong password.";
                return;
            }
        }
    }
?>

<html lang="en">
<head>
<meta charset="utf-8">
<style>
body {
    background-color: black;
    color: lightgray;
    font-family: sans-serif;
    text-align: center;
}
h1 {
    font-size: 2rem;
    margin-top: 30px;
}
button {
    background: black;
    color: magenta;
    padding: 8px;
}
button.text-button.text-button {
    font-size: 1rem;
    height: 4rem;
}
textarea {
    display: block;
    height: 1rem;
    overflow: hidden;
    resize: none;
    background: black;
    color: cyan;
}
div {
    display: inline-block;
}
hr {
    visibility: hidden;
}
input {
    margin: 0;
    vertical-align: middle;
}
</style>
</head>
<body>
    <h1>Info display</h1>
    <p>Use space to empty the display.</p>
    <hr>
    <label>Password:</label>
    <div style="vertical-align: top;">
        <textarea id="password"></textarea>
    </div>
    <hr>
    <label>Time settings:</label>
    <div>
        <input type="radio" name="hour" id="10" value="10"/>
        <label for="10">10</label> |
        <input type="radio" name="hour" id="11" value="11"/>
        <label for="11">11</label> |
        <input type="radio" name="hour" id="12" value="12" checked="true"/>
        <label for="12">12</label> |
        <input type="radio" name="hour" id="13" value="13"/>
        <label for="13">13</label> |
        <input type="radio" name="hour" id="14" value="14"/>
        <label for="14">14</label> |
        <input type="checkbox" id="half-hour"/>
        <label for="half-hour">30</label>
    </div>
    <hr>
    <label>Templates:</label>
    <div>
        <button id="template-coming" onclick="textTemplate(this)">Tulen klo</button>
        <button id="template-away" onclick="textTemplate(this)">Poissa</button>
        <button id="template-need" onclick="textTemplate(this)">Tarve</button>
        <button id="template-sick" onclick="textTemplate(this)">Sairaana</button>
    </div>
    <hr>
    <label>Text:</label>
    <div style="vertical-align: top;">
        <textarea id="line1" maxlength="16"></textarea>
        <textarea id="line2" maxlength="16"></textarea>
        <textarea id="line3" maxlength="16"></textarea>
        <textarea id="line4" maxlength="16"></textarea>
    </div>
    <button class="text-button" onclick="queueText()">Queue</button>
    <hr>
    <p id="result" style="color: white;"></p>

<script>
const addPreventNewLine = (elem_id) => {
    document.getElementById(elem_id).addEventListener("input", () => {
        document.getElementById(elem_id).value = document.getElementById(elem_id).value.replace(/\n/g,'');
    })
}
addPreventNewLine("line1");
addPreventNewLine("line2");
addPreventNewLine("line3");
addPreventNewLine("line4");

const queueText = async () => {
    const res = await fetch(window.location.href, {
        method: "POST",
        body: JSON.stringify({
            "password": document.getElementById("password").value,
            "infotext": document.getElementById("line1").value + "\n" +
                        document.getElementById("line2").value + "\n" +
                        document.getElementById("line3").value + "\n" +
                        document.getElementById("line4").value + "\n"
        }),
        headers: {
            "Content-type": "application/json; charset=UTF-8"
        }
    });
    document.getElementById("result").innerHTML = await res.text();
}

const getCurrentDate = () => {
    const d = new Date();
    return d.getDate() + "." + (d.getMonth()+1) + ".";
}

const getSetTime = () => {
    return document.querySelector('input[name="hour"]:checked').value + "." + (document.getElementById("half-hour").checked ? "30" : "00");
}

const templates = (id) => {
    switch (id) {
        case ("template-coming"):
            return [
                `${getCurrentDate()} tulen`,
                `noin kello ${getSetTime()}`,
                "Kiva, jos saisin",
                "paikan. <3 -Olli"
            ]
        case ("template-away"):
            return [
                `${getCurrentDate()} olen`,
                "toisaalla ja",
                "paikka VAPAA :)",
                "           -Olli"
            ]
        case ("template-need"):
            return [
                `${getCurrentDate()} tarvitsen`,
                "paikkaani ja",
                "haluaisin sen.",
                "Kiitos.    -Olli"
            ]
        case ("template-sick"):
            return [
                `${getCurrentDate()} olen`,
                "sairaslomalla ja",
                "paikka on VAPAA.",
                "           -Olli"
            ]
    }
}

const textTemplate = (elem) => {
    document.getElementById("line1").value = templates(elem.id)[0];
    document.getElementById("line2").value = templates(elem.id)[1];
    document.getElementById("line3").value = templates(elem.id)[2];
    document.getElementById("line4").value = templates(elem.id)[3];
}
</script>

</body>
</html>
