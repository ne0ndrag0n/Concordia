local WaterLevel = {
  level = nil,
  decay_rate = 10,
  decay_callback = nil
}

function WaterLevel:init( decay_rate )
  if type( decay_rate ) == "number" then
    self.decay_rate = decay_rate
  end

  self.level = 100

  self.decay_callback = bluebear.engine.queue_callback(
    bluebear.util.seconds_to_ticks( 5 ),
    bluebear.util.bind( self.decay, self )
  )
end

function WaterLevel:load( jsonString )
  local saved = bluebear.util.json.decode( jsonString )

  self.level = saved.level
end

function WaterLevel:save()
  return bluebear.util.json.encode( {
    level = self.level
  } )
end

function WaterLevel:close()
  bluebear.engine.cancel_callback( self.decay_callback )
end

function WaterLevel:decay()
  self.level = math.max( self.level - self.decay_rate, 0 )

  print( 'game.component.plant.water_level', 'Hello! I am a game.component.plant.water_level and I am now at '..tostring( self.level ) )

  self.decay_callback = bluebear.engine.queue_callback(
    bluebear.util.seconds_to_ticks( 5 ),
    bluebear.util.bind( self.decay, self )
  )
end

bluebear.entity.register_component( 'game.component.plant.water_level', WaterLevel )
