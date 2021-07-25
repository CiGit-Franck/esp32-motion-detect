const char ROOT_page[] PROGMEM = R"=====(
<html>
<head>
    <title>ESP32 Page</title>
    <style>
        body {
            background-color: powderblue;
        }

        h1 {
            font: bold cursive;
            color: blue;
            text-align: center;
        }

        p {
            color: red;
        }
    </style>
</head>

<body>
    <h1>Motion detect Page</h1>
    <p>Counter : %s</p>
</body>
</html>
)=====";