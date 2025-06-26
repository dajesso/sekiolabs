<?php
include("inc/config.php");
$page_title = "sekiolabs - moon";
$page_desc = "phase of the moon in ASCII";
include("inc/header.php");

$moon = shell_exec("phoon -l 32|texttohtml");
if (strpos($moon, "EE")) {
 $cssclass="greenmoon";
} else {
 $cssclass="moon";
}
echo '<div class="'. $cssclass . '">' . $moon . '</div>';
echo '<br /><a href="/">back home</a><br/>';

include("inc/footer.php"); ?>
