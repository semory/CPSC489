/*
** CPSC489 Game Development Project
** Copyright (c) 2018  Steven Emory <mobysteve@gmail.com>
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __CS489_ENTITY_MARKER_LIST_H
#define __CS489_ENTITY_MARKER_LIST_H

#include "errors.h"
#include "entity.h"
#include "en_entmark.h"

// Entity Marker List
class EntityMarkerList : public Entity {

 // Variables
 private :
  MeshInstance* instance;
  uint32 n_markers;
  std::unique_ptr<EntityMarker[]> markers;

 // Member Functions
 public :
  ErrorCode SetModelInstance(const STDSTRINGW& name);
  ErrorCode SetModelInstance(uint32 model);
  const MeshInstance* GetModelInstance(void)const;
  void SetMarkers(uint32 n, std::unique_ptr<EntityMarker[]>& data);
  const EntityMarker* GetMarkers(void)const;

 // Operators
 public :
  EntityMarker& operator [](size_t index);
  const EntityMarker& operator [](size_t index)const;

 // Events
 public :
  virtual void EvTrigger(Entity* source) {}

 // Special Member Functions
 public :
  EntityMarkerList();
  EntityMarkerList(const EntityMarkerList&) = delete;
  virtual ~EntityMarkerList();
  void operator =(const EntityMarkerList&) = delete;
};

#endif
