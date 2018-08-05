#pragma once

#include "LuaManager.h"
#include <filesystem>

class Script
{

public:
	Script() = default;
	Script(std::filesystem::path const& path);
	Script(std::string const& script);
	~Script() = default;

public:
	template <class type>
	const std::optional<type>	GetAttribute(std::string const& table, std::string const& attribute) const
	{
		US_NS_LUA;
		LuaRef tab = getGlobal(table.c_str());

		LuaRef attr = tab[attribute.c_str()];
		if (attr.isNil())
			return std::nullopt;

		return attr.cast<type>();
	}

};

class ScriptManager
{
public:
	ScriptManager() = default;
	~ScriptManager() = default;

	MAKE_SINGLETONE(ScriptManager);
	void Create();

public:
	void AddScript(std::string const& script)
	{
		m_scripts.emplace_back(std::make_unique<Script>(script));
	}


	void AddScript(std::filesystem::path const& path)
	{
		m_scripts.emplace_back(std::make_unique<Script>(path));
	}

	void AddScript(std::unique_ptr<Script>&& s)
	{
		m_scripts.emplace_back(std::move(s));
	}

	const std::unique_ptr<Script>& GetScript(int index) const
	{
		return m_scripts[index];
	}

private:
	std::vector<std::unique_ptr<Script>> m_scripts;

};