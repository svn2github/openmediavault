<?php
/**
 * This file is part of OpenMediaVault.
 *
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
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
require_once("openmediavault/rpc.inc");

class OC_User_OpenMediaVault extends OC_User_Backend {
	private $context = NULL;

	public function __construct() {
		$this->context = array(
			"username" => "admin",
			"role" => OMV_ROLE_ADMINISTRATOR
		);
	}

	/**
	 * @brief Check if the password is correct
	 * @param $uid The username
	 * @param $password The password
	 * @returns true/false
	 *
	 * Check if the password is correct without logging in the user
	 */
	public function checkPassword($uid, $password) {
		$valid = false;
		try {
			$result = OMVRpc::exec("UserMgmt", "authUser", array(
			  	  "username" => strtolower($uid),
			  	  "password" => $password
			  ), $this->context, OMV_RPC_MODE_REMOTE);
			$valid = $result['authenticated'];
		} catch(Exception $e) {
			OC_Log::write("OC_User_OpenMediaVault", sprintf(
			  "Failed to check password (code=%d, message=%s)",
			  $e->getCode(), $e->getMessage()), OC_Log::ERROR);
		}
		return $valid;
	}

	/**
	 * @brief check if a user exists
	 * @param string $uid the username
	 * @return boolean
	 */
	public function userExists($uid) {
		$exists = false;
		try {
			OMVRpc::exec("UserMgmt", "getUser", array(
			  	  "username" => strtolower($uid)
			  ), $this->context, OMV_RPC_MODE_REMOTE);
			$exists = true;
		} catch(Exception $e) {
			OC_Log::write("OC_User_OpenMediaVault", sprintf(
			  "Failed to get user '%s' (code=%d, message=%s)", $uid,
			  $e->getCode(), $e->getMessage()), OC_Log::ERROR);
		}
		return $exists;
	}

	/**
	 * @brief Create a new user
	 * @param $uid The username of the user to create
	 * @param $password The password of the new user
	 * @returns true/false
	 *
	 * Creates a new user. Basic checking of username is done in OC_User
	 * itself, not in its subclasses.
	 */
	public function createUser($uid, $password) {
		OC_Log::write("OC_User_OpenMediaVault", "Not possible to create ".
		  "users from web frontend using OpenMediaVault user backend",
		  OC_Log::ERROR);
		return false;
	}

	/**
	 * @brief delete a user
	 * @param $uid The username of the user to delete
	 * @returns true/false
	 *
	 * Deletes a user
	 */
	public function deleteUser($uid) {
		OC_Log::write("OC_User_OpenMediaVault", "Not possible to delete ".
		  "users from web frontend using OpenMediaVault user backend",
		  OC_Log::ERROR);
		return false;
	}

	/**
	 * @brief Check if a user list is available or not
	 * @return boolean if users can be listed or not
	 */
	public function hasUserListings() {
		return true;
	}

	/**
	* @brief Get a list of all users
	* @returns array with all uids
	*
	* Get a list of all users.
	*/
	public function getUsers($search = '', $limit = null, $offset = null) {
		$result = array();
		try {
			$users = OMVRpc::exec("UserMgmt", "enumerateUsers", NULL,
			  $this->context, OMV_RPC_MODE_REMOTE);
			foreach($users as $userk => $userv) {
				$result[] = $userv['name'];
			}
		} catch(Exception $e) {
			OC_Log::write("OC_User_OpenMediaVault", sprintf(
			  "Failed to get users (code=%d, message=%s)", $e->getCode(),
			  $e->getMessage()), OC_Log::ERROR);
		}
		return $result;
	}

	/**
	 * @brief Set password
	 * @param $uid The username
	 * @param $password The new password
	 * @returns true/false
	 *
	 * Change the password of a user
	 */
	public function setPassword($uid, $password) {
		OC_Log::write("OC_User_OpenMediaVault", "Not possible to change ".
		  "passwords from web frontend using OpenMediaVault user backend",
		  OC_Log::ERROR);
		return false;
	}
}
?>
