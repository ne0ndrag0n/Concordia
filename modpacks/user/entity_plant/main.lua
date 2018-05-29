bluebear.engine.require_modpack( 'component_water' )

-- TEST shit
local Demo = {
  instance = nil
}

function Demo:init()
  bluebear.event.register_key( 'x', function()
    self.instance = bluebear.world.place_object( 'floor', {} )
  end )

  bluebear.event.register_key( 'c', function()
    if self.instance then
      self.instance:set_current_animation( 'Armature|ArmatureAction' )
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
