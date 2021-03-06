<?php
/**
 * This file is part of OpenMediaVault.
 *
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @copyright Copyright (c) 2009-2014 Volker Theile
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
require_once("openmediavault/module.inc");
require_once("openmediavault/config.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/monit.inc");
require_once("openmediavault/initscript.inc");

class OMVModuleForkedDaapd extends OMVModuleServiceAbstract
  implements OMVINotifyListener, OMVIModuleServiceStatus {
	/**
	 * Get the module name.
	 */
	public function getName() {
		return "daap";
	}

	/**
	 * Defines the modules that if present should start before the service
	 * provided by this module.
	 * @return An array of modules.
	 */
	public function shouldStart() { return array("monit"); }

	/**
	 * Defines the modules that if present should be stopped after the
	 * service provided by this module.
	 * @return An array of modules.
	 */
	function shouldStop() { return array("monit"); }

	/**
	 * Generate the configuration.
	 * @throw E_EXEC_FAILED
	 */
	public function applyConfig() {
		$cmd = "omv-mkconf forked-daapd 2>&1";
		if(0 !== $this->exec($cmd, $output)) {
			throw new OMVException(OMVErrorMsg::E_EXEC_FAILED,
			  $cmd, implode("\n", $output));
		}
	}

	/**
	 * Stop the managed service.
	 */
	public function stopService() {
		// Disable monitoring of this service.
		$monit = new OMVMonit();
		$monit->unmonitor("forked-daapd", TRUE);
		// Stop this service and unregister System-V style init script.
		$initd = new OMVSysVInitScript("forked-daapd", FALSE);
		$initd->exec();
	}

	/**
	 * Start the managed service.
	 * @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	public function startService() {
		global $xmlConfig;
		$xpath = "//services/daap";
		$object = $xmlConfig->get($xpath);
		if(is_null($object)) {
			throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
			  $xpath);
		}
		if(FALSE === boolval($object['enable']))
			return;
		// Start this service and register System-V style init script.
		$initd = new OMVSysVInitScript("forked-daapd", TRUE);
		$initd->exec();
		// Enable monitoring of this service.
		$monit = new OMVMonit();
		$monit->monitor("forked-daapd");
	}

	/**
	 * Get the status of the managed service.
	 * @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	public function getStatus() {
		global $xmlConfig;
		$xpath = "//services/daap";
		$object = $xmlConfig->get($xpath);
		if(is_null($object)) {
			throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
			  $xpath);
		}
		$monit = new OMVMonit();
		return array(
			"name" => $this->getName(),
			"title" => gettext("iTunes/DAAP"),
			"enabled" => boolval($object['enable']),
			"running" => ($monit->status("forked-daapd") === "running")
		);
	}

	/**
	 * Helper function to find out whether the given shared folder
	 * configuration object is used. If it is used, then mark the
	 * module as dirty.
	 * @param type The event message type.
	 * @param path The event message path.
	 * @param object The configuration object.
	 */
	final public function onModifySharedFolder($type, $path, $object) {
		global $xmlConfig;
		$xpath = sprintf("//services/daap[sharedfolderref='%s']",
		  $object['uuid']);
		if(TRUE === $xmlConfig->exists($xpath))
			$this->setDirty();
	}

	/**
	 * Helper function that is called whenever the basic service settings
	 * have been modified.
	 * @param type The event message type.
	 * @param path The event message path.
	 * @param object The new configuration object.
	 * @param oldObject The previous configuration object.
	 * @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	final public function onModifySettings($type, $path, $object, $oldObject) {
		// Mark this module as dirty.
		$this->setDirty();
		// Check whether the monit daemon configuration needs to be updated.
		// In this case mark the module as dirty.
		if($oldObject['enable'] != $object['enable'])
			$this->setDirtyByName("monit");
	}

	/**
	 * Bind listeners.
	 */
	function bindListeners(OMVNotifyDispatcher $dispatcher) {
		$dispatcher->addListener(
		  OMV_NOTIFY_MODIFY,
		  "org.openmediavault.services.daap",
		  array($this, "onModifySettings"));
		$dispatcher->addListener(
		  OMV_NOTIFY_MODIFY,
		  "org.openmediavault.system.shares.sharedfolder",
		  array($this, "onModifySharedFolder"));
	}
}

// Register module.
$moduleMgr = &OMVModuleMgr::getInstance();
$moduleMgr->registerModule(new OMVModuleForkedDaapd());
?>
