function buscar() {
    var xmlhttp = new XMLHttpRequest();

    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == XMLHttpRequest.DONE && xmlhttp.status == 200) {
            console.log(xmlhttp.responseText);
            document.getElementById("redesId").innerHTML = xmlhttp.responseText;
        }
    };

    xmlhttp.open("GET", "buscar", true);
    xmlhttp.send();
}

document.getElementById("btnBuscar").addEventListener("click", function() {
    console.log("btnBuscar");
    buscar();
});