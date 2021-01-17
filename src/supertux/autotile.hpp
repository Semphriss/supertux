//  SuperTux
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_SUPERTUX_AUTOTILE_HPP
#define HEADER_SUPERTUX_SUPERTUX_AUTOTILE_HPP

#include <string>
#include <vector>

class ReaderMapping;
class TileMap;
class Writer;

/**
 * Class that represents one possible mask for an autotile. A mask is a set of
 * rules concerning the neighboring tiles of a given tile, which will determine
 * which autotile is appropriate to use.
 */
class AutotileMask
{
public:
  struct MaskPosition
  {
    int x, y;
    bool solid;
    std::string set;
  };

public:
  AutotileMask();

  /** Parses a mask from a ReaderMapping object. */
  static AutotileMask from_reader(ReaderMapping& reader);

  /** Saves the mask to the given Writer. */
  void save(Writer& writer);

  /**
   * Validates everything in the current mask.
   * 
   * @param errors A pointer to a list of strings which will be populated with
   *               all errors found. If null, errors will be printed with
   *               log_warning.
   * @returns Whether or not the collection is clean. False if there are errors.
   */
  bool validate(std::vector<std::string>* errors);

  /**
   * Checks if the given tile corresponds to this mask.
   * 
   * @param map The tilemap on which to perform the operation.
   * @param x The horizontal position of the tile to autotile.
   * @param y The vertical position of the tile to autotile.
   * @returns true if this mask matches the given tile, false otherwise.
   */
  bool matches(TileMap& map, int x, int y);

public:
  std::vector<MaskPosition> m_rules;
};

/**
 * Class that represents a single contextual autotile, a tile in the context
 * of autotiling.
 */
class Autotile
{
public:
  Autotile();

  /** Makes the link between an autotile and a tile in the tileset. */
  struct AutotileTile
  {
    /** The ID of the tile in the tileset. */
    uint32_t id;

    /**
     * The chances of this tile of occuring. The real chances are this number
     * divided by the sum of all weights in the autotile.
     */
    float weight;
  };

  /** Parses an autotile from a ReaderMapping object. */
  static Autotile from_reader(ReaderMapping& reader);

  /** Saves the autotile to the given Writer. */
  void save(Writer& writer);

  /**
   * Validates everything in the current autotile.
   * 
   * @param errors A pointer to a list of strings which will be populated with
   *               all errors found. If null, errors will be printed with
   *               log_warning.
   * @returns Whether or not the collection is clean. False if there are errors.
   */
  bool validate(std::vector<std::string>* errors);

  /**
   * Checks if the given tile corresponds to this autotile.
   * 
   * @param map The tilemap on which to perform the operation.
   * @param x The horizontal position of the tile to autotile.
   * @param y The vertical position of the tile to autotile.
   * @returns true if this autotile matches the given tile, false otherwise.
   */
  bool matches(TileMap& map, int x, int y);

public:
  bool m_solid;
  std::vector<AutotileTile> m_tiles;
  std::vector<AutotileMask> m_masks;
};

/**
 * Class that represents a "substance" from an autotile perspective. Tiles that
 * commonly autotile with each other are part of the same autotileset.
 */
class AutotileSet
{
public:
  AutotileSet();

  /** Parses an autotileset from a ReaderMapping object. */
  static AutotileSet from_reader(ReaderMapping& reader);

  /** Saves the autotileset to the given Writer. */
  void save(Writer& writer);

  /** Returns true if the current Autotileset contains this tile. */
  bool contains_tile(uint32_t tile);

  /**
   * Validates everything in the current autotileset.
   * 
   * @param errors A pointer to a list of strings which will be populated with
   *               all errors found. If null, errors will be printed with
   *               log_warning.
   * @returns Whether or not the collection is clean. False if there are errors.
   */
  bool validate(std::vector<std::string>* errors);

  /**
   * Sets the correct tile at the given position.
   * 
   * @param map The tilemap on which to perform the operation.
   * @param x The horizontal position of the tile to autotile.
   * @param y The vertical position of the tile to autotile.
   */
  void autotile(TileMap& map, int x, int y);

public:
  std::string m_name;
  uint32_t m_default_tile;
  std::vector<Autotile> m_autotiles;
};

/**
 * Class that contains a full autotile configuration file. There is one
 * autotileset collection per tileset.
 */
class AutotileSetCollection
{
public:
  /**
   * Struct that represents groups of autotilesets. Only used internally, to make
   * the job easier for autotile masks.
   */
  struct AutotileSetGroup
  {
    std::string name;
    std::vector<std::string> set_names;
  };

public:
  AutotileSetCollection();

  /** Parses a full autotileset collection from a file. */
  static AutotileSetCollection from_file(std::string file);

  /** Parses a full autotileset collection from a ReaderMapping object. */
  static AutotileSetCollection from_reader(ReaderMapping& reader);

  /** Saves the full autotileset collection to the given Writer. */
  void save(Writer& writer);

  /** Returns all autotilesets which contain the given tile. */
  std::vector<AutotileSet> get_sets_from_tile(uint32_t tile);

  /**
   * Sets the correct tile at the given position.
   * 
   * @param map The tilemap on which to perform the operation.
   * @param x The horizontal position of the tile to autotile.
   * @param y The vertical position of the tile to autotile.
   */
  void autotile(TileMap& map, int x, int y);

  /**
   * Sets the correct tile at the given position.
   * 
   * @param map The tilemap on which to perform the operation.
   * @param x The horizontal position of the corner to autotile. (0, 0 is top-left)
   * @param y The vertical position of the corner to autotile.
   * @param tile The tile that's being placed. Autotileset will be deduced automatically.
   */
  void autotile_corner(TileMap& map, int x, int y, uint32_t tile);

  /**
   * Validates everything in the current collection.
   * 
   * @param errors A pointer to a list of strings which will be populated with
   *               all errors found. If null, errors will be printed with
   *               log_warning.
   * @returns Whether or not the collection is clean. False if there are errors.
   */
  bool validate(std::vector<std::string>* errors);

public:
  std::vector<AutotileSet> m_autotilesets;
  std::vector<AutotileSetGroup> m_groups;
};

#endif

/* EOF */
