<?php
include("inc/config.php");
$page_title = "sekiolabs - main";
$page_desc = "home of sekiolabs on the internet";
include("inc/header.php");

$date_pst = trim(shell_exec('TZ="America/Vancouver" date'));
$date_acst = trim(shell_exec('TZ="Australia/Adelaide" date'));
$date_utc = trim(shell_exec('date -u'));
$uname = trim(shell_exec('uname -a'));


$apiKey = "9a09d198710095b77a6a95d33e7f1a6e";
$cityId = "2078024";
$googleApiUrl = "https://api.openweathermap.org/data/2.5/weather?id=" . $cityId . "&lang=en&units=metric&APPID=" . $apiKey;

$ch = curl_init();

curl_setopt($ch, CURLOPT_HEADER, 0);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_URL, $googleApiUrl);
curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
curl_setopt($ch, CURLOPT_VERBOSE, 0);
curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
#$weather_aus = curl_exec($ch);

$response = curl_exec($ch);

$data = json_decode($response);


$weather_aus = "Weather in Adelaide: " .  $data->main->temp_max . '°C';


$cityId2 = "6173331";


$apiKey2 = "9a09d198710095b77a6a95d33e7f1a6e";
$googleApiUrl2 = "https://api.openweathermap.org/data/2.5/weather?id=" . $cityId2 . "&lang=en&units=metric&APPID=" . $apiKey2;



$ch2 = curl_init();

curl_setopt($ch2, CURLOPT_HEADER, 0);
curl_setopt($ch2, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch2, CURLOPT_URL, $googleApiUrl2);
curl_setopt($ch2, CURLOPT_FOLLOWLOCATION, 1);
curl_setopt($ch2, CURLOPT_VERBOSE, 0);
curl_setopt($ch2, CURLOPT_SSL_VERIFYPEER, false);
#$weather_aus = curl_exec($ch);

$response2 = curl_exec($ch2);

$data2 = json_decode($response2);


$weather_can = "Weather in Vancouver: " .  $data2->main->temp_max . '°C';


#$weather_can = trim(shell_exec("ansiweather -l Vancouver,CA -a false | texttohtml"));
#$weather_aus = trim(shell_exec("ansiweather -l Adelaide,AU -a false | texttohtml"));
$fortune = nl2br(trim(shell_exec('/usr/games/fortune -s -n 200')));
$uptime = trim(shell_exec('uptime'));

?>
<div class="status"><?php
echo $uname."\n";
echo $uptime."\n\n";
echo $date_pst ."\n".$date_utc."\n".$date_acst."\n";
?></div><br />
<div class="weather"><?php
echo $weather_can;
echo "\n";
echo $weather_aus;
?></div><br />
      <div class="content-table">
        <div class="content-cell">
          <div class="list-body">
            <div class="list-header">
              Services
            </div>
            <div class="list-entry"> <!-- not really a toy  -sekio sep3/2024 -->
              <a href="/pub">Public files</a>
            </div>
            <div class="list-entry">
              <a href="https://mail.sekiolabs.net">Webmail</a>
            </div>
            <div class="list-entry">
              <a href="ssh://sekiolabs.net">Secure shell access</a>
            </div>
          </div>
        </div>
        <div class="content-cell">
          <div class="list-body">
            <div class="list-header">
              Toys
            </div>
            <div class="list-entry">
              <a href="/moon.php">Moon phases</a>
            </div>
            <div class="list-entry">
              <a href="/globe.php">Globe</a>
            </div>
            <div class="list-entry">
              <a href="https://www.felicis.gay">Felicis the Panda</a>
            </div>
            <div class="list-entry">
              <a href="https://www.felicisin.space">Felicis in space!</a>
            </div>
          </div>
        </div>
        <div class="content-cell">
          <div class="list-body">
            <div class="list-header">
              Other
            </div>
            <div class="list-entry">
              <a href="mailto:sekio@sekiolabs.net">Contact sekio</a>
            </div>
            <div class="list-entry">
              <a href="mailto:sekio64@gmail.com">Contact sekio (GMail)</a>
            </div>
            <div class="list-entry">
             
            </div>
          </div>
        </div>
      </div><br />
      <p>This is a mirror of sekios website RIP buddy me and felicis miss you <3</p>
      <div class="fortune">
Oh, love is real enough, you will find it some day, but it has one
arch-enemy -- and that is life.
-- Jean Anouilh, "Ardele"</div>
<?php include("inc/footer.php"); ?>
