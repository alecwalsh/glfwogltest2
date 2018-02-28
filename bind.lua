deltaTime = 0
lastTime = 0
RotSpeed = 0
reverse = false

lambdaCalled = false
cubeAdded = false

function runAtTime(time, func)
    local run = false
    checkEvents[#checkEvents+1] = function()
        if cpp.elapsedTime > time and not run then
            run = true
            func()
        end
    end
end

checkEvents = setmetatable({}, {__call=function() for k,v in ipairs(checkEvents) do v() end end})

function init()
    runAtTime(5, AddCube)
end
-- runAtTime(5, AddCube)

function loop()
    deltaTime = cpp.elapsedTime - lastTime
    lastTime = cpp.elapsedTime
    
--     if cpp.elapsedTime > 10 and not cubeAdded then
--         cubeAdded = true
--         AddCube()
--     end
    
    
    if not reverse then
        RotSpeed = RotSpeed + deltaTime
    else
        RotSpeed = RotSpeed - deltaTime
    end
    
    if RotSpeed > 5 and not reverse then
        reverse = true
    end
    
    if RotSpeed <= 0.5 and reverse then
        reverse = false
    end
    
    checkEvents()
    
    cpp.RotSpeed = RotSpeed
    Tick()
    if not lambdaCalled then
        LambdaTest()
        lambdaCalled = true
    end
end
