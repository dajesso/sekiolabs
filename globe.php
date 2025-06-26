<?php
include("inc/config.php");
$page_title = "sekiolabs - globe";
$page_desc="current view of the globe in ASCII";
include("inc/header.php");

$globe = shell_exec("globe|texttohtml");
$date = shell_exec("date");
$date_utc = shell_exec("date -u");

$padding = '         ';
$cssclass="globe";

echo '<div class="'. $cssclass . '">' . $padding . $date
      . $padding . $date_utc . "\n" . $globe . '</div>';

echo '<br /><a href="/">back home</a><br/>';

include("inc/footer.php"); ?>
