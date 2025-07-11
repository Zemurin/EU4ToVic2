#include "ProvinceNameParser.h"
#include "../../Configuration.h"
#include "OSCompatibilityLayer.h"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

V2::ProvinceNameParser::ProvinceNameParser()
{
	if (commonItems::DoesFileExist("blankMod/output/localisation/text.csv"))
	{
		importProvinceLocalizations("blankMod/output/localisation/text.csv");
	}
	const auto& locFolder = theConfiguration.getVic2Path() / "localisation";
	if (commonItems::DoesFolderExist(locFolder))
	{
		for (const auto& locFile: commonItems::GetAllFilesInFolderRecursive(locFolder))
		{
			importProvinceLocalizations(locFolder / locFile);
		}
	}
	if (theConfiguration.isHpmEnabled())
	{
		importProvinceLocalizations(theConfiguration.getVanillaVic2Path() / "localisation/text.csv");
	}
}

void V2::ProvinceNameParser::importProvinceLocalizations(const std::filesystem::path& file)
{
	std::ifstream locFile(file);

	while (locFile.good() && !locFile.eof())
	{
		std::string line;
		getline(locFile, line);
		if (!(line.substr(0, 4) == "PROV") || !isdigit(line[4]))
			continue;

		const auto position = line.find_first_of(';');
		const auto provID = stoi(line.substr(4, position - 4));
		const auto name = line.substr(position + 1, line.find_first_of(';', position + 1) - position - 1);

		provinceNames.insert(std::make_pair(provID, name));
	}
	locFile.close();
}

std::optional<std::string> V2::ProvinceNameParser::getProvinceName(int provID) const
{
	const auto& name = provinceNames.find(provID);
	if (name != provinceNames.end())
		return name->second;
	return std::nullopt;
}
