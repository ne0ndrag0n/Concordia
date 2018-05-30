bluebear.engine.require_modpack( 'component_water' )

-- TEST shit
local Demo = {
  instance = nil
}

function Demo:init()
  bluebear.event.register_key( 'x', function()
    if not self.instance then
      self.instance = bluebear.world.place_object( self:get_entity(), 'floor', {} )
    end
  end )

  bluebear.event.register_key( 'c', function()
    if self.instance then
      self.instance:set_current_animation( 'Armature|ArmatureAction' )
    end
  end )

  bluebear.event.register_key( 'v', function()
    if self.instance then
      bluebear.world.remove_object( self.instance )
      self.instance = nil
    end
  end )
end

bluebear.entity.register_component( 'game.component.plant_keys', Demo )

bluebear.entity.register_entity( 'game.entity.plant',
  {
    'system.component.model_manager',
    'game.component.plant.water_level',
    'game.component.plant_keys'
  }
)
