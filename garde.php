<html>
<head>
<title>Gardevoirs</title>
<link rel="stylesheet" type="text/css" href="/style.css" />
</head>
<body>
<?php

$gallery_dir = "/var/www/html/garde";
$web_dir = "garde";
$thumbs_dir = "thumbs";
$goodies = scandir($gallery_dir);

foreach ($goodies as $fn) {
 if ($fn == ".") continue;
 if ($fn == "..") continue;

 $info = pathinfo($fn);
 $file_name =  basename($fn,'.'.$info['extension']);

// echo '<a href="' . $web_dir . '/' . $fn .'"><img src="' . $web_dir .
//       '/' . $fn .'">' . $fn . '<br />'. $fn .'</a><br />';
 echo '<span class="image">'."\n".
      ' <a href="' . $web_dir . '/' . $fn .'">'."\n".
      '  <img src="' . $web_dir . '/' . $thumbs_dir . '/'.  $file_name .
      '_thumb.png" />'."\n </a>\n</span>\n";
}

?>

</body>
</html>
