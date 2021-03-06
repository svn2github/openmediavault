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

Ext.ns("OMV.data");

/**
 * @class OMV.data.DataRequest
 */
OMV.data.DataRequest = function() {
};
OMV.data.DataRequest.prototype = {
	/**
	 * Initialize the hidden IFrame used to handle download
	 * requests.
	 * @param url The URL of the download script.
	 */
	init : function(url) {
		this.iframe = Ext.getBody().createChild({
			tag: "iframe",
			src: url
		});
		this.iframe.setStyle("visibility", "hidden");
		this.iframe.setStyle("display", "none");
	},

	/**
	 * Sends a download request to a remote server.
	 * @param service The name/class of the service to be executed.
	 * @param method The method name to be executed.
	 * @param params The parameters of the method to be executed as object.
	 * @return None
	 */
	request : function(service, method, params) {
		var form = this.iframe.dom.contentDocument.forms[0];
		form.service.setAttribute("value", service);
		form.method.setAttribute("value", method);
		// Additional parameters must be encoded as string because they are
		// submitted as POST parameters. They will be decoded by the RPC
		// implementation automatically.
		form.params.setAttribute("value", Ext.util.JSON.encode(params).
		  htmlspecialchars());
		// Submit form to send download request.
		form.submit();
	}
}
