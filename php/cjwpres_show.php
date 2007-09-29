<?php
require('cjwpres.php');

$jid = $_GET['jid'];

if(!isset($jid))
	die('Need the jid argument');

$status = getStatus($jid);

echo '<table border="1"><thead style="font-weight: bold"><tr><td>Resource</td><td>Priority</td><td>Status</td><td>Message</td></tr></thead>';
echo "<tbody>\n";

foreach($status as $presence)
{
	$presence[message] = nl2br($presence[message]);
	echo '<tr><td>'.$presence[resource].'</td>';
	echo '<td>'.$presence[priority].'</td>';
	echo '<td>'.$presence[status].'</td>';
	echo '<td>'.$presence[message].'</td></tr>';
	echo "\n";
}

echo '</tbody></table>';

?>
