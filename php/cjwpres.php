<?php

function getStatus($jid)
{
	$result = array();
	$response = $name = $port = null;

	$socket = socket_create(AF_INET, SOCK_DGRAM, 0);
	socket_sendto($socket, $jid, strlen($jid), 0, '127.0.0.1', 9999);
	
	$readfds = array($socket);
	$writefds = $excptfds = null;
	if(socket_select($readfds, $writefds, $excptfds, 5) > 0)
	{
		socket_recvfrom($socket, $response, 65535, 0, $name, $port);
		
		$lines = explode("\n", $response);
		foreach($lines as $line)
		{
			if(strlen($line) == 0)
				continue;
			list($resource, $priority, $status, $message) = explode("\t", $line);
			$message = str_replace(array('\n', '\t'), array("\n", "\t"), $message);
			$result[] = array('resource' => $resource, 'priority' => $priority, 'status' => $status, 'message' => $message);
		}
	}
	
	return $result;
}

?>
