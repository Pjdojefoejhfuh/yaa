--!strict
--[[ Crosshair — Ninja.cc Edition
Watermark: Ninja.cc · Palette: noir/blanc
Auto-loads Yokai kill sound script
]]

local Players, RunService, UserInputService, GuiService =
    game:GetService("Players"),
    game:GetService("RunService"),
    game:GetService("UserInputService"),
    game:GetService("GuiService")

local TargetGui = game:GetService("CoreGui") or
    (Players.LocalPlayer:WaitForChild("PlayerGui", 5) :: PlayerGui)

if TargetGui:FindFirstChild("SleepyStaticCenterCrosshair") then
    TargetGui["SleepyStaticCenterCrosshair"]:Destroy()
end

local ScreenGui = Instance.new("ScreenGui")
ScreenGui.Name, ScreenGui.ResetOnSpawn, ScreenGui.IgnoreGuiInset,
    ScreenGui.DisplayOrder, ScreenGui.Parent =
    "SleepyStaticCenterCrosshair", false, true, 9999, TargetGui

local CenterContainer = Instance.new("Frame")
CenterContainer.Name, CenterContainer.Size, CenterContainer.Position,
    CenterContainer.BackgroundTransparency, CenterContainer.Parent =
    "Centro", UDim2.fromOffset(0, 0), UDim2.fromScale(1, 1), 1, ScreenGui

-- =======================================================================
-- CONFIG
-- =======================================================================
local CFG = {
    WatermarkText = "Ninja.cc",
    Length = 22,
    Thick = 1,
    Gap = 12,
    RotSpd = 100,
    PulseSpd = 3.5,
    SizeRng = 10,
    RGBSpeed = 0.5,
    GlowMult = 5
}

local Transparencies = {0.66, 0.75, 0.84, 0.91, 0.97}

local DIRECTIONS = {
    Vector2.new(0, -1),
    Vector2.new(0, 1),
    Vector2.new(-1, 0),
    Vector2.new(1, 0)
}

local Components, RNG = {}, Random.new()

for i, dir in ipairs(DIRECTIONS) do
    local isHoriz = dir.X ~= 0

    local Outline = Instance.new("Frame")
    Outline.Name = string.format("Borde_%d", i)
    Outline.BorderSizePixel, Outline.BackgroundColor3,
        Outline.BackgroundTransparency, Outline.AnchorPoint, Outline.ZIndex,
        Outline.Parent =
        0, Color3.fromRGB(0, 0, 0), 0.5, Vector2.new(0.5, 0.5), 10,
        CenterContainer

    local Line = Instance.new("Frame")
    Line.Name = string.format("Linea_%d", i)
    Line.BorderSizePixel, Line.AnchorPoint, Line.ZIndex, Line.Parent =
        0, Vector2.new(0.5, 0.5), 11, CenterContainer

    local Layers = table.create(#Transparencies)
    for step, trans in ipairs(Transparencies) do
        local Glow = Instance.new("Frame")
        Glow.Name = string.format("GlowLayer_%d_%d", i, step)
        Glow.BorderSizePixel, Glow.AnchorPoint, Glow.BackgroundTransparency,
            Glow.ZIndex, Glow.Parent =
            0, Vector2.new(0.5, 0.5), trans, step, CenterContainer

        local Corner = Instance.new("UICorner")
        Corner.CornerRadius = UDim.new(1, 0)
        Corner.Parent = Glow

        table.insert(Layers, Glow)
    end

    table.insert(Components, {
        Line = Line,
        Outline = Outline,
        Glows = Layers,
        Dir = dir,
        IsHorizontal = isHoriz
    })
end

local Watermark = Instance.new("TextLabel")
Watermark.Name = "NinjaWatermark"
Watermark.Text = CFG.WatermarkText
Watermark.Font = Enum.Font.Arcade
Watermark.TextSize = 19
Watermark.Size = UDim2.fromOffset(150, 20)
Watermark.AnchorPoint = Vector2.new(0.5, 0)
Watermark.BackgroundTransparency = 1
Watermark.ZIndex = 12

local TextStroke = Instance.new("UIStroke")
TextStroke.Thickness = 1
TextStroke.Color = Color3.fromRGB(0, 0, 0)
TextStroke.Transparency = 0.4
TextStroke.Parent = Watermark

Watermark.Parent = ScreenGui

local TickCounter, ModoMenu, SmoothPos, SmoothRot = 100, false, Vector2.new(0, 0), 0

UserInputService.InputBegan:Connect(function(input)
    if input.KeyCode == Enum.KeyCode.M or input.KeyCode == Enum.KeyCode.ButtonSelect then
        ModoMenu = not ModoMenu
    end
end)

local Connection: RBXScriptConnection
Connection = RunService.PreRender:Connect(function(dt)
    if not CenterContainer or not CenterContainer.Parent then
        Connection:Disconnect()
        return
    end

    TickCounter += dt

    local TargetPos = UserInputService:GetMouseLocation()

    SmoothPos = SmoothPos:Lerp(TargetPos, 1 - math.exp(-25 * dt))
    CenterContainer.Position = UDim2.fromOffset(SmoothPos.X, SmoothPos.Y)

    local Pulse = math.sin(TickCounter * CFG.PulseSpd)
    local DynamicLength = CFG.Length + (Pulse * CFG.SizeRng)

    SmoothRot = (SmoothRot + (CFG.RotSpd * (0.35 + ((Pulse + 1) * 0.5 * (1.75 - 0.35)))) * dt) % 360
    CenterContainer.Rotation = SmoothRot

    local ColorA = Color3.fromRGB(255, 255, 255)
    local ColorB = Color3.fromRGB(0, 0, 0)

    local colorAlpha = (math.sin(TickCounter * 2) + 1) * 0.5
    local CurrentColor = ColorA:Lerp(ColorB, colorAlpha)
    local ScatX, ScatY = RNG:NextNumber(-0.35, 0.35), RNG:NextNumber(-0.5, 0.5)

    for _, comp in ipairs(Components) do
        local centerOffset = CFG.Gap + (DynamicLength * 0.5)
        local posX, posY = comp.Dir.X * centerOffset, comp.Dir.Y * centerOffset
        local sizeW, sizeH = comp.IsHorizontal and DynamicLength or CFG.Thick,
            comp.IsHorizontal and CFG.Thick or DynamicLength

        comp.Line.Size, comp.Line.Position, comp.Line.BackgroundColor3 =
            UDim2.fromOffset(sizeW, sizeH),
            UDim2.fromOffset(posX, posY),
            CurrentColor

        comp.Outline.Size, comp.Outline.Position =
            UDim2.fromOffset(sizeW + (comp.IsHorizontal and 0 or 2),
                sizeH + (comp.IsHorizontal and 2 or 0)),
            UDim2.fromOffset(posX, posY)

        for step, glow in ipairs(comp.Glows) do
            local spread = step * CFG.GlowMult
            local gW = sizeW + (comp.IsHorizontal and (spread * 0.2) or spread) +
                (comp.IsHorizontal and ScatY or ScatX)
            local gH = sizeH + (comp.IsHorizontal and spread or (spread * 0.2)) +
                (comp.IsHorizontal and ScatX or ScatY)

            glow.Size = UDim2.fromOffset(gW, gH)
            glow.Position = UDim2.fromOffset(
                posX + (comp.IsHorizontal and (ScatY * 0.2) or ScatX),
                posY + (comp.IsHorizontal and ScatX or (ScatY * 0.2))
            )
            glow.BackgroundColor3 = CurrentColor
        end
    end

    Watermark.Text = CFG.WatermarkText

    local MaxLineLength = CFG.Length + CFG.SizeRng
    local FixedBottomOffset = CFG.Gap + MaxLineLength
    Watermark.Position = UDim2.fromOffset(SmoothPos.X, SmoothPos.Y + FixedBottomOffset + 0)
    Watermark.TextColor3 = CurrentColor
end)

-- =======================================================================
-- AUTO-LOAD YOKAI KILL SOUND
-- =======================================================================
task.spawn(function()
    local ok, err = pcall(function()
        loadstring(game:HttpGet("https://raw.githubusercontent.com/SUSU67Hub/yokai-kill-sound-/refs/heads/main/Yokai"))()
    end)
    if not ok then
        warn("[Ninja.cc] Yokai load failed: " .. tostring(err))
    end
end)
