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
#include "stdafx.h"
#include "entity.h"

uint32 Entity::baseID = 0;
std::map<uint32, Entity*> Entity::entity_map = std::map<uint32, Entity*>();

Entity::Entity()
{
 // Base Properties
 id = baseID;
 baseID++;
 entity_map.insert(std::make_pair(id, this));
 for(int i = 0; i < 16; i++) targets[i] = nullptr;

 // Positioning Properties
 position.reset(0.0f, 0.0f, 0.0f);
 orientation.load_identity();

 // Mesh Properties
 mesh = nullptr;
}

Entity::~Entity()
{
 entity_map.erase(id);
}

Entity* Entity::GetEntity(uint32 eid)
{
 auto iter = entity_map.find(eid);
 if(iter == entity_map.end()) return nullptr;
 return iter->second;
}

void Entity::Render(void)
{
 // nothing to do
 if(!mesh) return;

 // by default we could render an axis object here
}