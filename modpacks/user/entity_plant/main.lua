bluebear.engine.require_modpack( 'component_water' )

-- TEST shit
local Demo = {
  instance = nil
}

function Demo:init()
  bluebear.event.register_key( 'x', function()
    if not self.instance then
      self.instance = self:get_model_manager():place_object( 'armaturebox', {} )

     local interaction = bluebear.util.types.Interaction.new( "animate", "Animate", bluebear.util.bind( self.play_animation, self ) )
     local interaction2 = bluebear.util.types.Interaction.new( "move", "Move", bluebear.util.bind( self.move_object, self ) )
     local interaction3 = bluebear.util.types.Interaction.new( "remove", "Remove", bluebear.util.bind( self.remove_object, self ) )
     self:get_entity():find_components( 'system.component.interaction_set' )[ 1 ]:associate_interaction( self.instance, interaction )
     self:get_entity():find_components( 'system.component.interaction_set' )[ 1 ]:associate_interaction( self.instance, interaction2 )
     self:get_entity():find_components( 'system.component.interaction_set' )[ 1 ]:associate_interaction( self.instance, interaction3 )
    end
  end )

  bluebear.event.register_key( 'c', bluebear.util.bind( self.play_animation, self ) )
  bluebear.event.register_key( 'v', bluebear.util.bind( self.remove_object, self ) )
  bluebear.event.register_key( 'b', bluebear.util.bind( self.move_object, self ) )
end

function Demo:play_animation()
  if self.instance then
    self.instance:set_current_animation( 'Armature|ArmatureAction' )
  end
end

function Demo:remove_object()
  if self.instance then
    self:get_model_manager():remove_object( self.instance )
    self.instance = nil
  end
end

function Demo:move_object()
  if self.instance then
    self.instance:get_transform():set_position( bluebear.util.types.Vec3.new( -5.0, 0.0, 0.0 ) )
  end
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
