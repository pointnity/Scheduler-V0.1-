<?php

$q = strtolower($_GET["term"]);
if (!$q) return;
$items = array(
"Great Bittern"=>"Botaurus stellaris",
"Little Grebe"=>"Tachybaptus ruficollis",
