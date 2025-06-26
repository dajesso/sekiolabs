<?php
if (empty($sekiolabs)) die('Need to be included');

$cur_url = $_SERVER['SCRIPT_NAME'];
if (empty($page_title)) $page_title = "sekiolabs";
if (empty($page_desc)) $page_desc = "home of sekiolabs";

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"  "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title><?php echo $page_title; ?></title>
  <link rel="stylesheet" href="/style.css" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <meta name="description" content=<?php echo '"'.$page_desc.'"'; ?>/>
</head>
<body>
  <div class="centre">
    <div class="logo">
     <a href="/"><img src="/images/logo.png" alt="sekiolabs" /></a>
    </div>
    <br />
    <div class="content"><!-- begin user content -->
