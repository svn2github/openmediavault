<?php
/**
 * This file is part of OpenMediaVault.
 *
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @copyright Copyright (c) 2009-2012 Volker Theile
 *
 * OpenMediaVault is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * OpenMediaVault is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenMediaVault. If not, see <http://www.gnu.org/licenses/>.
 */
require_once("openmediavault/object.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/util.inc");
require_once("openmediavault/rpc.inc");
require_once("openmediavault/notify.inc");

class ForkedDaapdRpc extends OMVRpc {
	public function __construct() {
		$this->methodSchemata = array(
			"set" => array('{
				"type":"object",
				"properties":{
					"enable":{"type":"boolean"},
					"libraryname":{"type":"string"},
					"port":{"type":"integer","minimum":0,"maximum":65535},
					"sharedfolderref":{'.
					  $GLOBALS['OMV_JSONSCHEMA_UUID_NONE'].'},
					"passwordrequired":{"type":"boolean"},
					"password":{"type":"string"}
				}
			}')
		);
	}

	/**
	 * Get configuration object.
	 */
	function get() {
		global $xmlConfig;
		// Check permissions
		$this->validateSession();
		if (!$this->hasRole(OMV_ROLE_ADMINISTRATOR)) {
			throw new OMVException(OMVErrorMsg::E_RPC_SERVICE_INVALID_PERMISSION);
		}
		$this->commitSession();
		// Get configuration object
		$xpath = "//services/daap";
		$object = $xmlConfig->get($xpath);
		if (is_null($object)) {
			throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
			  $xpath);
		}
		// Modify result data
		$object['enable'] = boolval($object['enable']);
		$object['port'] = intval($object['port']);
		$object['passwordrequired'] = boolval($object['passwordrequired']);
		return $object;
	}

	/**
	 * Set configuration object.
	 */
	function set($data) {
//		$this->debug(var_export(func_get_args(), TRUE));
		global $xmlConfig;
		// Check permissions
		$this->validateSession();
		if (!$this->hasRole(OMV_ROLE_ADMINISTRATOR)) {
			throw new OMVException(OMVErrorMsg::E_RPC_SERVICE_INVALID_PERMISSION);
		}
		$this->commitSession();
		// Validate method parameters
		$this->validateParams(__METHOD__, func_get_args());
		// Prepare configuration data
		$object = array(
			"enable" => array_boolval($data, 'enable'),
			"libraryname" => $data['libraryname'],
			"port" => $data['port'],
			"sharedfolderref" => $data['sharedfolderref'],
			"passwordrequired" => array_boolval($data, 'passwordrequired'),
			"password" => $data['password'],
			"adminpassword" => "unused"
		);
		// Set configuration object
		$xpath = "//services/daap";
		if (FALSE === $xmlConfig->replace($xpath, $object)) {
			throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);
		}
		// Save configuration
		if (FALSE === $xmlConfig->save()) {
			throw new OMVException(OMVErrorMsg::E_CONFIG_SAVE_FAILED,
			  $xmlConfig->getError());
		}
		// Notify configuration changes
		$dispatcher = &OMVNotifyDispatcher::getInstance();
		$dispatcher->notify(OMV_NOTIFY_MODIFY,
		  "org.openmediavault.services.daap", $object);
	}
}
?>
