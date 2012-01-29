/*
    OpenSR - opensource multi-genre game based upon "Space Rangers 2: Dominators"
    Copyright (C) 2011 - 2012 Kosyak <ObKo@mail.ru>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "JSONHelper.h"
#include "Log.h"
#include "Texture.h"
#include <boost/shared_ptr.hpp>
#include "ResourceManager.h"
#include <libRanger.h>
#include <vector>

//TODO: Need error handling.
namespace Rangers
{
TextureRegion JSONHelper::parseTextureRegion(const Json::Value& object)
{
    if (!object.isObject())
    {
        Log::warning() << "Invalid JSON object.";
        return TextureRegion(boost::shared_ptr<Texture>(), 0, 0, 0, 0);
    }
    std::wstring textureName = fromUTF8(object.get("texture", "").asString().c_str());
    boost::shared_ptr<Texture> texture;

    if (!textureName.empty())
        texture = ResourceManager::instance()->loadTexture(textureName);

    if (!texture)
        return TextureRegion(boost::shared_ptr<Texture>(), 0, 0, 0, 0);

    int x, y, width, height;

    x = object.get("x", 0).asInt();
    y = object.get("y", 0).asInt();
    width = object.get("width", texture->width()).asInt();
    height = object.get("height", texture->height()).asInt();

    return TextureRegion(texture, x, y, width, height);
}

NinePatch JSONHelper::parseNinePatch(const Json::Value& object)
{
    if (!object.isObject())
    {
        Log::warning() << "Invalid JSON object.";
        return NinePatch();
    }
    int rows = object.get("rows", 0).asInt();
    int columns = object.get("columns", 0).asInt();
    std::vector<int> sizeableRows;
    std::vector<int> sizeableColumns;
    std::vector<TextureRegion> regions;
    Json::Value array = object.get("sizeableRows", 0);
    Json::Value::iterator end = array.end();
    for (Json::Value::iterator i = array.begin(); i != end; ++i)
    {
        sizeableRows.push_back((*i).asInt());
    }
    array = object.get("sizeableColumns", 0);
    end = array.end();
    for (Json::Value::iterator i = array.begin(); i != end; ++i)
    {
        sizeableColumns.push_back((*i).asInt());
    }
    array = object.get("regions", 0);
    end = array.end();
    for (Json::Value::iterator i = array.begin(); i != end; ++i)
    {
        regions.push_back(parseTextureRegion(*i));
    }
    return NinePatch(regions, rows, columns, sizeableRows, sizeableColumns);
}

NinePatch JSONHelper::parseNinePatch(const std::string& json)
{
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(json, root))
    {
        Log::error() << "Error parsing JSON: " << reader.getFormatedErrorMessages();
        return NinePatch();
    }
    Json::Value::Members members = root.getMemberNames();
    if (std::find(members.begin(), members.end(), "NinePatch") == root.getMemberNames().end())
    {
        Log::error() << "Invalid NinePatch JSON";
        return NinePatch();
    }

    return parseNinePatch(root.get("NinePatch", 0).begin().operator * ());
}
}