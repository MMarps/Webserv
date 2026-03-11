#!/usr/bin/php-cgi
<?php

header("Content-Type: text/html; charset=UTF-8");

$name = isset($_GET['name']) ? htmlspecialchars($_GET['name'], ENT_QUOTES, 'UTF-8') : 'inconnu';

echo "<!DOCTYPE html>";
echo "<html lang='fr'>";
echo "<head>";
echo "    <meta charset='UTF-8'>";
echo "    <title>Test CGI PHP</title>";
echo "</head>";
echo "<body>";
echo "    <h1>Bonjour, $name !</h1>";
echo "    <p>Ce script PHP fonctionne en mode CGI.</p>";
echo "</body>";
echo "</html>";
?>