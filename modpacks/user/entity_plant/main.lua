bluebear.engine.require_modpack( 'component_water' )

-- TEST shit
local Demo = {
  instance = nil
}

function Demo:init()
  bluebear.event.register_key( 'x', function()
    if not self.instance then
      self.instance = self:get_model_manager():place_object( 'armaturebox', {} )

     local interaction = bluebear.util.types.Interaction.new( "id", "Label", function() print( 'called' ) end )
      self:get_entity():find_components( 'system.component.interaction_set' )[ 1 ]:associate_interaction( self.instance, interaction )
    end
  end )

  bluebear.event.register_key( 'c', function()
    if self.instance then
      self.instance:set_current_animation( 'Armature|ArmatureAction' )
    end
  end )

  bluebear.event.register_key( 'v', function()
    if self.instance then
      self:get_model_manager():remove_object( self.instance )
      self.instance = nil
    end
  end )

  bluebear.event.register_key( 'b', function()
    if self.instance then
      self.instance:get_transform():set_position( bluebear.util.types.Vec3.new( 0.0, 5.0, 0.0 ) )
    end
  end )
end

function Demo:get_model_manager()
  local matches = self:get_entity():find_components( 'system.component.model_manager' )

  if #matches > 0 then
    return matches[ 1 ]
  else
    return nil
  end
end

bluebear.entity.register_component( 'game.component.plant_keys', Demo )

bluebear.entity.register_entity( 'game.entity.plant',
  {
    'system.component.model_manager',
    'game.component.plant.water_level',
    'game.component.plant_keys'
  }
)
