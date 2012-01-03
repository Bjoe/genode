/*
 * \brief  Access to process configuration
 * \author Norman Feske
 * \date   2010-05-04
 */

/*
 * Copyright (C) 2010-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__OS__CONFIG_H_
#define _INCLUDE__OS__CONFIG_H_

#include <util/xml_node.h>
#include <dataspace/client.h>
#include <rom_session/connection.h>
#include <base/exception.h>

namespace Genode {

	class Config
	{
		private:

			Rom_connection       _config_rom;
			Dataspace_capability _config_ds;
			Xml_node             _config_xml;

		public:

			/**
			 * Exception class for configuration errors
			 */
			class Invalid : public Exception { };

			/**
			 * Constructor
			 */
			Config() :
				_config_rom("config"),
				_config_ds(_config_rom.dataspace()),
				_config_xml(env()->rm_session()->attach(_config_ds),
			                Genode::Dataspace_client(_config_ds).size())
			{ }

			Xml_node xml_node() { return _config_xml; }
	};

	/**
	 * Return singleton instance of config
	 */
	static Config *config()
	{
		static bool config_failed = false;
		if (!config_failed) {
			try {
				static Config config_inst;
				return &config_inst;
			} catch (Genode::Rom_connection::Rom_connection_failed) {
				PERR("Could not obtain config file");
			} catch (Genode::Xml_node::Invalid_syntax) {
				PERR("Config file has invalid syntax");
			}
		}
		/* do not try again to construct 'config_inst' */
		config_failed = true;
		throw Config::Invalid();
	}
}

#endif /* _INCLUDE__OS__CONFIG_H_ */
