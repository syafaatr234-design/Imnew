local WindUI = loadstring(game:HttpGet("https://github.com/Footagesus/WindUI/releases/latest/download/main.lua"))()

local ReplicatedStorage = game:GetService("ReplicatedStorage")
local RunService = game:GetService("RunService")
local UserInputService = game:GetService("UserInputService")
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer
local HttpService = game:GetService("HttpService")

local Character = LocalPlayer.Character or LocalPlayer.CharacterAdded:Wait()
local Humanoid = Character:WaitForChild("Humanoid")

local Packages = ReplicatedStorage:WaitForChild("Packages")
local NetIndex = Packages:WaitForChild("_Index"):WaitForChild("sleitnick_net@0.2.0")
local Net = NetIndex:WaitForChild("net")

local ChargeFishingRod = Net:WaitForChild("RF/ChargeFishingRod")
local RequestFishingMinigameStarted = Net:WaitForChild("RF/RequestFishingMinigameStarted")
local FishingCompleted = Net:WaitForChild("RE/FishingCompleted")
local EquipToolFromHotbar = Net:WaitForChild("RE/EquipToolFromHotbar")
local SellAllItems = Net:WaitForChild("RF/SellAllItems")
local CancelFishingInputs = Net:WaitForChild("RF/CancelFishingInputs")
local ActivateEnchantingAltar = Net:WaitForChild("RE/ActivateEnchantingAltar")
local UpdateOxygen = Net:WaitForChild("URE/UpdateOxygen")

local FishingController = require(ReplicatedStorage.Controllers.FishingController)

-- Infinite Jump
UserInputService.JumpRequest:Connect(function()
    if _G.InfiniteJump and LocalPlayer.Character then
        local CharHumanoid = LocalPlayer.Character:FindFirstChildOfClass("Humanoid")
        if CharHumanoid then
            CharHumanoid:ChangeState(Enum.HumanoidStateType.Jumping)
        end
    end
end)

LocalPlayer.CharacterAdded:Connect(function(NewCharacter)
    local NewHumanoid = NewCharacter:WaitForChild("Humanoid")
    NewHumanoid.UseJumpPower = true
    NewHumanoid.JumpPower = _G.CustomJumpPower or 50
end)

-- UI Creation
local MainWindow = WindUI:CreateWindow({
    Title = "Blueflare",
    Icon = "door-open",
    Author = "CRACKED BY Project X",
    Folder = "Blueflarev2",
    Size = UDim2.fromOffset(580, 460),
    MinSize = Vector2.new(560, 350),
    MaxSize = Vector2.new(850, 560),
    Transparent = true,
    Theme = "Dark",
    Resizable = true,
    SideBarWidth = 200,
    BackgroundImageTransparency = 0.42,
    HideSearchBar = false,
    ScrollBarEnabled = true,
    User = {
        Enabled = true,
        Anonymous = false,
        Callback = function()
            print("clicked")
        end
    }
})

-- Tabs
local Tabs = {
    Home = MainWindow:Tab({Title = "Home", Icon = "cat"}),
    Fishing = MainWindow:Tab({Title = "Fishing", Icon = "dog"}),
    Shop = MainWindow:Tab({Title = "Shop", Icon = "piggy-bank"}),
    Quest = MainWindow:Tab({Title = "Quest", Icon = "bird"}),
    Utility = MainWindow:Tab({Title = "Utility", Icon = "rat"}),
    Teleport = MainWindow:Tab({Title = "Teleport", Icon = "rabbit"}),
    Misc = MainWindow:Tab({Title = "Misc", Icon = "turtle"})
}

-- Home Tab
Tabs.Home:Section({Title = "Support", TextXAlignment = "Left", TextSize = 17})
Tabs.Home:Button({
    Title = "Discord Server",
    Desc = "click to copy link",
    Callback = function()
        if setclipboard then
            setclipboard("https://discord.gg/fUNMX29UAJ")
        end
    end
})

Tabs.Home:Section({Title = "User Settings", TextXAlignment = "Left", TextSize = 17})

Tabs.Home:Input({
    Title = "WalkSpeed",
    Desc = "Minimum 16 speed",
    Value = "16",
    InputIcon = "bird",
    Type = "Input",
    Placeholder = "Enter number...",
    Callback = function(Value)
        local Speed = tonumber(Value)
        if Speed and Speed >= 16 then
            Humanoid.WalkSpeed = Speed
        else
            Humanoid.WalkSpeed = 16
        end
    end
})

Tabs.Home:Input({
    Title = "Jump Power",
    Desc = "Minimum 50 jump",
    Value = "50",
    InputIcon = "bird",
    Type = "Input",
    Placeholder = "Enter number...",
    Callback = function(Value)
        local JumpPower = tonumber(Value)
        if JumpPower then
            _G.CustomJumpPower = JumpPower
            local Char = LocalPlayer.Character
            if Char then
                local CharHumanoid = Char:FindFirstChildOfClass("Humanoid")
                if CharHumanoid then
                    CharHumanoid.UseJumpPower = true
                    CharHumanoid.JumpPower = JumpPower
                end
            end
        end
    end
})

Tabs.Home:Button({
    Title = "Reset Speed & Jump",
    Desc = "Return to normal (16 speed, 50 jump)",
    Callback = function()
        Humanoid.WalkSpeed = 16
        _G.CustomJumpPower = 50
        local Char = LocalPlayer.Character
        if Char then
            local CharHumanoid = Char:FindFirstChildOfClass("Humanoid")
            if CharHumanoid then
                CharHumanoid.UseJumpPower = true
                CharHumanoid.JumpPower = 50
            end
        end
    end
})

Tabs.Home:Toggle({
    Title = "Infinite Jump",
    Desc = "activate to use infinite jump",
    Icon = "bird",
    Type = "Checkbox",
    Default = false,
    Callback = function(Value)
        _G.InfiniteJump = Value
    end
})

-- Fishing Tab
Tabs.Fishing:Section({Title = "Legit", TextXAlignment = "Left", TextSize = 17})

local AutoLegitFishEnabled = false
local FishingInProgress = false
local AutoFishThread = nil
local OriginalGetPower = nil
local PlayerGui = LocalPlayer:WaitForChild("PlayerGui")
local Camera = workspace.CurrentCamera
local VirtualInputManager = game:GetService("VirtualInputManager")

local function ClickScreen()
    local ViewportSize = Camera.ViewportSize
    local X = ViewportSize.X * 0.95
    local Y = ViewportSize.Y * 0.95
    VirtualInputManager:SendMouseButtonEvent(X, Y, 0, true, nil, 0)
    VirtualInputManager:SendMouseButtonEvent(X, Y, 0, false, nil, 0)
end

local function StopAutoFish()
    FishingInProgress = false
    if OriginalGetPower then
        FishingController._getPower = OriginalGetPower
    end
end

local function AutoFishLoop()
    pcall(function()
        while AutoLegitFishEnabled do
            if not LocalPlayer.Character then
                LocalPlayer.CharacterAdded:Wait()
            end
            if not AutoLegitFishEnabled then break end
            
            if EquipToolFromHotbar then
                pcall(EquipToolFromHotbar.FireServer, EquipToolFromHotbar, 1)
            end
            
            task.wait(0.1)
            
            if not FishingInProgress then
                ClickScreen()
                FishingInProgress = true
            end
            
            local FishingUI = PlayerGui:FindFirstChild("Fishing")
            if FishingUI then
                FishingUI = FishingUI:FindFirstChild("Main")
            end
            
            if FishingUI and FishingUI.Visible then
                for i = 1, 20 do
                    if not AutoLegitFishEnabled then break end
                    ClickScreen()
                    task.wait(0.02)
                end
            end
            
            task.wait(0.1)
        end
    end)
    StopAutoFish()
end

local function ToggleAutoLegitFish(Enabled)
    AutoLegitFishEnabled = Enabled
    if Enabled then
        if not OriginalGetPower then
            OriginalGetPower = FishingController._getPower
        end
        FishingController._getPower = function() return 1 end
        FishingInProgress = false
        if AutoFishThread then
            task.cancel(AutoFishThread)
        end
        AutoFishThread = task.spawn(AutoFishLoop)
        print("Auto Legit Fish: Started")
    else
        StopAutoFish()
        if AutoFishThread then
            task.cancel(AutoFishThread)
            AutoFishThread = nil
        end
    end
end

Tabs.Fishing:Toggle({
    Title = "Auto Legit Fish",
    Desc = "Automatically fishes for you",
    Type = "Checkbox",
    Default = false,
    Callback = function(Value)
        ToggleAutoLegitFish(Value)
    end
})

Tabs.Fishing:Section({Title = "Fishing", TextXAlignment = "Left", TextSize = 17})

local InstantFishingEnabled = false
local CompleteDelay = 0.1
local CustomCompleteDelay = 1

local function EquipFishingRod()
    pcall(function()
        EquipToolFromHotbar:FireServer(1)
    end)
end

local function InstantFishLoop()
    if InstantFishingEnabled then
        pcall(function()
            ChargeFishingRod:InvokeServer(1756863567.217075)
            RequestFishingMinigameStarted:InvokeServer(-139.63796997070312, 0.9964792798079721)
        end)
        
        task.wait(CompleteDelay)
        
        pcall(function()
            CancelFishingInputs:InvokeServer()
        end)
        
        if InstantFishingEnabled then
            pcall(function()
                ChargeFishingRod:InvokeServer(1756863567.217075)
                RequestFishingMinigameStarted:InvokeServer(-139.63796997070312, 0.9964792798079721)
            end)
            
            task.wait(CustomCompleteDelay)
            
            pcall(function()
                FishingCompleted:FireServer()
            end)
            
            task.spawn(InstantFishLoop)
        end
    end
end

local function StartInstantFishing()
    if not InstantFishingEnabled then
        InstantFishingEnabled = true
        EquipFishingRod()
        task.wait(0.5)
        task.spawn(InstantFishLoop)
    end
end

local function StopInstantFishing()
    InstantFishingEnabled = false
end

Tabs.Fishing:Toggle({
    Title = "Instant Fishing",
    Desc = "Another Instant Fishing If Instant Fishing Fixed",
    Type = "Checkbox",
    Callback = function(Value)
        if Value then
            StartInstantFishing()
        else
            StopInstantFishing()
        end
    end
})

local DelayInput = Tabs.Fishing:Input({
    Title = "Custom Complete Delay",
    Desc = "Enter delay in seconds",
    Value = tostring(CustomCompleteDelay),
    InputIcon = "timer",
    Type = "Input",
    Placeholder = "Enter number...",
    Callback = function(Value)
        local Delay = tonumber(Value)
        if Delay and Delay >= 0 then
            CustomCompleteDelay = Delay
        end
    end
})

-- Shop Tab
Tabs.Shop:Section({Title = "Shop", TextXAlignment = "Left", TextSize = 17})

Tabs.Shop:Button({
    Title = "Sell All Items",
    Desc = "Sell all items in your inventory",
    Callback = function()
        pcall(function()
            SellAllItems:InvokeServer()
        end)
    end
})

-- Quest Tab
Tabs.Quest:Section({Title = "Quest", TextXAlignment = "Left", TextSize = 17})

local AutoQuestEnabled = false
local AutoQuestThread = nil

local function GetQuestProgress()
    local QuestData = {}
    
    local MenuRings = workspace:FindFirstChild("!!! MENU RINGS")
    if not MenuRings then return QuestData end
    
    for _, Ring in pairs(MenuRings:GetChildren()) do
        if Ring:IsA("Model") and Ring:FindFirstChild("Board") then
            local Board = Ring.Board
            if Board and Board:FindFirstChild("Gui") then
                local Gui = Board.Gui
                if Gui and Gui:FindFirstChild("Content") then
                    local Content = Gui.Content
                    local QuestText = ""
                    
                    for _, Child in pairs(Content:GetChildren()) do
                        if Child:IsA("TextLabel") and Child.Name ~= "Header" then
                            QuestText = QuestText .. Child.Text .. "\n"
                        end
                    end
                    
                    QuestData[Ring.Name] = QuestText
                end
            end
        end
    end
    
    return QuestData
end

local QuestProgressParagraph = Tabs.Quest:Paragraph({
    Title = "Quest Progress",
    Desc = "Loading quest data..."
})

task.spawn(function()
    while task.wait(5) do
        local QuestData = GetQuestProgress()
        local DisplayText = ""
        
        for QuestName, Progress in pairs(QuestData) do
            DisplayText = DisplayText .. "[" .. QuestName .. "]\n" .. Progress .. "\n\n"
        end
        
        if DisplayText == "" then
            DisplayText = "No active quests found"
        end
        
        QuestProgressParagraph:SetDesc(DisplayText)
    end
end)

-- Utility Tab
Tabs.Utility:Section({Title = "Utility", TextXAlignment = "Left", TextSize = 17})

local AutoFavoriteEnabled = false
local SelectedTier = "Legendary"
local TierSettings = {
    ["Artifact Items"] = {Ids = {265, 266, 267, 271}},
    Legendary = {TierName = "Legendary"},
    Mythic = {TierName = "Mythic"},
    Secret = {TierName = "SECRET"}
}

local function FavoriteItems(Tier)
    -- This function would need proper Data service implementation
    -- Placeholder for favorite functionality
    print("Auto Favorite: " .. Tier)
end

Tabs.Utility:Dropdown({
    Title = "Favorite Tier",
    Desc = "Select which item type or rarity you want to auto-favorite.",
    Values = {"Artifact Items", "Legendary", "Mythic", "Secret"},
    Default = "Legendary",
    Multi = false,
    Callback = function(Value)
        SelectedTier = Value
        WindUI:Notify({
            Title = "Favorite Tier Selected",
            Description = "Now set to favorite: " .. SelectedTier,
            Duration = 2
        })
    end
})

Tabs.Utility:Toggle({
    Title = "Auto Favorite",
    Desc = "Automatically favorite selected tier in your inventory.",
    Type = "Checkbox",
    Default = false,
    Callback = function(Value)
        AutoFavoriteEnabled = Value
        if Value then
            task.spawn(function()
                while AutoFavoriteEnabled do
                    FavoriteItems(SelectedTier)
                    task.wait(10)
                end
            end)
        end
    end
})

local OxygenBypassEnabled = false
local OxygenThread = nil

local function StartOxygenBypass()
    if not OxygenThread then
        OxygenBypassEnabled = true
        OxygenThread = coroutine.create(function()
            while OxygenBypassEnabled do
                UpdateOxygen:FireServer(-9999)
                wait(0.5)
            end
        end)
        coroutine.resume(OxygenThread)
    end
end

local function StopOxygenBypass()
    OxygenBypassEnabled = false
    OxygenThread = nil
end

Tabs.Utility:Toggle({
    Title = "Oxygen Bypass",
    Desc = "Cant Die",
    Icon = "shield",
    Type = "Checkbox",
    Default = false,
    Callback = function(Value)
        if Value then
            StartOxygenBypass()
        else
            StopOxygenBypass()
        end
    end
})

Tabs.Utility:Toggle({
    Title = "Freeze Character",
    Desc = "For Help Instant Fishing",
    Type = "Checkbox",
    Default = false,
    Callback = function(Value)
        _G.FreezeCharacter = Value
        if Value then
            local Char = game.Players.LocalPlayer.Character
            local RootPart = Char and Char:FindFirstChild("HumanoidRootPart")
            if RootPart then
                local originalCFrame = RootPart.CFrame
                local freezeConnection = game:GetService("RunService").Heartbeat:Connect(function()
                    if _G.FreezeCharacter and RootPart then
                        RootPart.CFrame = originalCFrame
                    end
                end)
            end
        elseif freezeConnection then
            freezeConnection:Disconnect()
            freezeConnection = nil
        end
    end
})

-- Teleport Tab
Tabs.Teleport:Section({Title = "Teleport", TextXAlignment = "Left", TextSize = 17})

local Locations = {
    ["Esoteric Island"] = Vector3.new(1990, 5, 1398),
    ["Kohana"] = Vector3.new(-603, 3, 719),
    ["Coral Refs"] = Vector3.new(-2855, 47, 1996),
    ["Enchant Room"] = Vector3.new(3221, -1303, 1406),
    ["Spawn"] = Vector3.new(33, 9, 2810),
    ["Volcano"] = Vector3.new(-632, 55, 197),
    ["Treasure Room"] = Vector3.new(-3602.01, -266.57, -1577.18),
    ["Sisyphus Statue"] = Vector3.new(-3703.69, -135.57, -1017.17),
    ["Crater Island Top"] = Vector3.new(1011.29, 22.68, 5076.27),
    ["Crater Island Ground"] = Vector3.new(1079.57, 3.64, 5080.35),
    ["Coral Reefs 1"] = Vector3.new(-3031.88, 2.52, 2276.36),
    ["Coral Reefs 2"] = Vector3.new(-3270.86, 2.5, 2228.1),
    ["Coral Reefs 3"] = Vector3.new(-3136.1, 2.61, 2126.11),
    ["Lost Shore"] = Vector3.new(-3737.97, 5.43, -854.68),
    ["Weather Machine"] = Vector3.new(-1524.88, 2.87, 1915.56),
    ["Kohana Volcano"] = Vector3.new(-561.81, 21.24, 156.72),
    ["Kohana 1"] = Vector3.new(-367.77, 6.75, 521.91),
    ["Kohana 2"] = Vector3.new(-623.96, 19.25, 419.36),
    ["Stingray Shores"] = Vector3.new(44.41, 28.83, 3048.93),
    ["Tropical Grove"] = Vector3.new(-2018.91, 9.04, 3750.59),
    ["Ice Sea"] = Vector3.new(2164, 7, 3269),
    ["Tropical Grove Cave 1"] = Vector3.new(-2151, 3, 3671),
    ["Tropical Grove Cave 2"] = Vector3.new(-2018, 5, 3756),
    ["Tropical Grove Highground"] = Vector3.new(-2139, 53, 3624),
    ["Fisherman Island Underground"] = Vector3.new(-62, 3, 2846),
    ["Fisherman Island Mid"] = Vector3.new(33, 3, 2764),
    ["Fisherman Island Left"] = Vector3.new(-26, 10, 2686),
    ["Fisherman Island Right"] = Vector3.new(95, 10, 2684),
    ["Jungle"] = Vector3.new(1491.21667, 6.35540199, -848.057617),
    ["Temple Guardian"] = Vector3.new(1481.58691, 127.624985, -596.321777),
    ["Underground Cellar"] = Vector3.new(2113.85693, -91.1985855, -699.206787),
    ["Sacred Temple"] = Vector3.new(1478.45508, -21.8498955, -630.773315)
}

Tabs.Teleport:Dropdown({
    Title = "Select Location",
    Values = {"Esoteric Island", "Kohana", "Kohana Volcano", "Kohana 1", "Kohana 2", "Coral Refs", "Enchant Room", "Tropical Grove", "Weather Machine", "Spawn", "Coral Refs 1", "Coral Reefs 2", "Coral Reefs 3", "Volcano", "Sisyphus Statue", "Treasure Room", "Crater Island Top", "Crater Island Ground", "Lost Shore", "Stingray Shores", "Tropical Grove", "Ice Sea", "Tropical Grove Cave 1", "Tropical Grove Cave 2", "Tropical Grove Highground", "Fisherman Island Underground", "Fisherman Island Mid", "Fisherman Island Left", "Fisherman Island Right", "Jungle", "Temple Guardian", "Underground Cellar", "Sacred Temple"},
    Callback = function(Location)
        if LocalPlayer.Character and LocalPlayer.Character:FindFirstChild("HumanoidRootPart") then
            LocalPlayer.Character.HumanoidRootPart.CFrame = CFrame.new(Locations[Location])
        end
    end
})

-- Misc Tab
Tabs.Misc:Section({Title = "Misc", TextXAlignment = "Left", TextSize = 17})

local HideIdentifierEnabled = false
local OverheadGui = LocalPlayer.Character:WaitForChild("HumanoidRootPart"):WaitForChild("Overhead")
local HeaderLabel = OverheadGui.Content.Header
local LevelLabel = OverheadGui.LevelContainer.Label

local OriginalName = HeaderLabel.Text
local OriginalLevel = LevelLabel.Text
local CustomName = OriginalName
local CustomLevel = OriginalLevel

Tabs.Misc:Input({
    Title = "Hide Name",
    Placeholder = "â€¢â€¢â€¢â€¢",
    Default = OriginalName,
    Callback = function(Value)
        CustomName = Value
        if HideIdentifierEnabled then
            HeaderLabel.Text = CustomName
        end
    end
})

Tabs.Misc:Input({
    Title = "Hide Level",
    Placeholder = "â€¢â€¢â€¢â€¢",
    Default = OriginalLevel,
    Callback = function(Value)
        CustomLevel = Value
        if HideIdentifierEnabled then
            LevelLabel.Text = CustomLevel
        end
    end
})

Tabs.Misc:Toggle({
    Title = "Start Hide Identifier",
    Default = false,
    Callback = function(Value)
        HideIdentifierEnabled = Value
        if Value then
            HeaderLabel.Text = CustomName
            LevelLabel.Text = CustomLevel
        else
            HeaderLabel.Text = OriginalName
            LevelLabel.Text = OriginalLevel
            HeaderLabel.TextColor3 = Color3.new(1, 1, 1)
            LevelLabel.TextColor3 = Color3.new(1, 1, 1)
        end
    end
})

-- Rainbow text effect
coroutine.wrap(function()
    local Hue = 0
    while true do
        if HideIdentifierEnabled then
            Hue = (Hue + 0.01) % 1
            local RainbowColor = Color3.fromHSV(Hue, 1, 1)
            HeaderLabel.TextColor3 = RainbowColor
            LevelLabel.TextColor3 = RainbowColor
        else
            HeaderLabel.TextColor3 = Color3.new(1, 1, 1)
            LevelLabel.TextColor3 = Color3.new(1, 1, 1)
        end
        wait(0.05)
    end
end)()

Tabs.Misc:Section({Title = "Other", TextXAlignment = "Left", TextSize = 17})

Tabs.Misc:Toggle({
    Title = "AntiAFK",
    Desc = "Prevent Roblox from kicking you when idle",
    Icon = "shield",
    Type = "Checkbox",
    Default = false,
    Callback = function(Value)
        _G.AntiAFK = Value
        local VirtualUser = game:GetService("VirtualUser")
        task.spawn(function()
            while _G.AntiAFK do
                task.wait(60)
                pcall(function()
                    VirtualUser:CaptureController()
                    VirtualUser:ClickButton2(Vector2.new())
                end)
            end
        end)
    end
})

Tabs.Misc:Toggle({
    Title = "Auto Reconnect",
    Desc = "Automatic reconnect if disconnected",
    Icon = "plug-zap",
    Type = "Checkbox",
    Default = false,
    Callback = function(Value)
        _G.AutoReconnect = Value
        if Value then
            task.spawn(function()
                while _G.AutoReconnect do
                    task.wait(2)
                    local PromptGui = game:GetService("CoreGui"):FindFirstChild("RobloxPromptGui")
                    local PromptOverlay = PromptGui and PromptGui:FindFirstChild("promptOverlay")
                    if PromptOverlay then
                        local ReconnectButton = PromptOverlay:FindFirstChild("ButtonPrimary")
                        if ReconnectButton and ReconnectButton.Visible then
                            firesignal(ReconnectButton.MouseButton1Click)
                        end
                    end
                end
            end)
        end
    end
})
