-- Main UI System dengan Tab dan Key System
local Creator = require("path.to.modules.Creator")
local New = Creator.New
local Tween = Creator.Tween

-- Import modules
local TabModule = require("path.to.ui.Tab")
local OpenButtonModule = require("path.to.ui.OpenButton")
local Button = require("path.to.elements.Button")
local Toggle = require("path.to.elements.Toggle")
local Input = require("path.to.elements.Input")
local Dropdown = require("path.to.elements.Dropdown")
local KeySystemModule = require("path.to.ui.KeySystem")

local UISystem = {
    Window = nil,
    OpenButton = nil,
    Tabs = {},
    CurrentTab = nil,
    IsOpen = false,
    RequiresKey = false,
    KeyVerified = false
}

-- Create main UI
function UISystem:Create(config)
    local WindowConfig = config.Window or {
        Title = config.Title or "UI System",
        Icon = config.Icon or "home",
        Size = UDim2.new(0, 500, 0, 600),
        Position = UDim2.new(0.5, 0, 0.5, 0),
        Parent = game:GetService("Players").LocalPlayer:WaitForChild("PlayerGui"),
        RequiresKey = config.RequiresKey or false,
        KeyConfig = config.KeyConfig or nil
    }
    
    -- Create screen gui
    local ScreenGui = New("ScreenGui", {
        Name = "MainUISystem",
        Parent = WindowConfig.Parent,
        ResetOnSpawn = false,
        ZIndexBehavior = Enum.ZIndexBehavior.Sibling
    })
    
    -- Main window frame
    local MainFrame = New("Frame", {
        Name = "MainWindow",
        Size = WindowConfig.Size,
        Position = WindowConfig.Position,
        AnchorPoint = Vector2.new(0.5, 0.5),
        BackgroundColor3 = Color3.fromRGB(25, 25, 30),
        BackgroundTransparency = 0.05,
        Visible = false,
        Active = true,
        Parent = ScreenGui
    }, {
        New("UICorner", {
            CornerRadius = UDim.new(0, 12)
        }),
        New("UIStroke", {
            Color = Color3.fromRGB(50, 50, 55),
            Thickness = 1
        }),
        New("UIPadding", {
            PaddingTop = UDim.new(0, 8),
            PaddingBottom = UDim.new(0, 8),
            PaddingLeft = UDim.new(0, 8),
            PaddingRight = UDim.new(0, 8)
        })
    })
    
    -- Top bar
    local TopBar = New("Frame", {
        Name = "TopBar",
        Size = UDim2.new(1, 0, 0, 40),
        BackgroundColor3 = Color3.fromRGB(35, 35, 40),
        BorderSizePixel = 0,
        Parent = MainFrame
    }, {
        New("UICorner", {
            CornerRadius = UDim.new(0, 8)
        }),
        New("TextLabel", {
            Name = "Title",
            Text = WindowConfig.Title,
            Size = UDim2.new(1, -80, 1, 0),
            Position = UDim2.new(0, 12, 0, 0),
            BackgroundTransparency = 1,
            TextColor3 = Color3.fromRGB(240, 240, 240),
            TextSize = 16,
            Font = Enum.Font.GothamSemibold,
            TextXAlignment = Enum.TextXAlignment.Left
        }),
        New("TextButton", {
            Name = "CloseButton",
            Size = UDim2.new(0, 32, 0, 32),
            Position = UDim2.new(1, -36, 0.5, 0),
            AnchorPoint = Vector2.new(0, 0.5),
            BackgroundColor3 = Color3.fromRGB(50, 50, 55),
            Text = "",
            AutoButtonColor = false
        }, {
            New("UICorner", {
                CornerRadius = UDim.new(0, 6)
            }),
            New("ImageLabel", {
                Image = "rbxassetid://7072725342", -- X icon
                Size = UDim2.new(0, 16, 0, 16),
                Position = UDim2.new(0.5, 0, 0.5, 0),
                AnchorPoint = Vector2.new(0.5, 0.5),
                BackgroundTransparency = 1,
                ImageColor3 = Color3.fromRGB(200, 200, 200)
            })
        }),
        New("TextButton", {
            Name = "MinimizeButton",
            Size = UDim2.new(0, 32, 0, 32),
            Position = UDim2.new(1, -72, 0.5, 0),
            AnchorPoint = Vector2.new(0, 0.5),
            BackgroundColor3 = Color3.fromRGB(50, 50, 55),
            Text = "",
            AutoButtonColor = false
        }, {
            New("UICorner", {
                CornerRadius = UDim.new(0, 6)
            }),
            New("ImageLabel", {
                Image = "rbxassetid://7072707365", -- Minimize icon
                Size = UDim2.new(0, 16, 0, 16),
                Position = UDim2.new(0.5, 0, 0.5, 0),
                AnchorPoint = Vector2.new(0.5, 0.5),
                BackgroundTransparency = 1,
                ImageColor3 = Color3.fromRGB(200, 200, 200)
            })
        })
    })
    
    -- Tabs container
    local TabsContainer = New("ScrollingFrame", {
        Name = "TabsContainer",
        Size = UDim2.new(1, 0, 0, 50),
        Position = UDim2.new(0, 0, 0, 45),
        BackgroundTransparency = 1,
        ScrollBarThickness = 0,
        CanvasSize = UDim2.new(0, 0, 0, 0),
        Parent = MainFrame
    }, {
        New("UIListLayout", {
            FillDirection = Enum.FillDirection.Horizontal,
            Padding = UDim.new(0, 8),
            SortOrder = Enum.SortOrder.LayoutOrder
        })
    })
    
    -- Content area
    local ContentArea = New("Frame", {
        Name = "ContentArea",
        Size = UDim2.new(1, 0, 1, -100),
        Position = UDim2.new(0, 0, 0, 100),
        BackgroundTransparency = 1,
        ClipsDescendants = true,
        Parent = MainFrame
    })
    
    -- Store references
    UISystem.Window = {
        Main = MainFrame,
        ScreenGui = ScreenGui,
        TabsContainer = TabsContainer,
        ContentArea = ContentArea,
        Config = WindowConfig
    }
    
    -- Initialize Tab Module
    TabModule:Init(UISystem.Window)
    
    -- Setup drag functionality
    UISystem:SetupDrag(TopBar)
    
    -- Setup button events
    UISystem:SetupEvents()
    
    -- Check if key is required
    if WindowConfig.RequiresKey and WindowConfig.KeyConfig then
        UISystem.RequiresKey = true
        UISystem:CheckKeySystem(WindowConfig.KeyConfig)
    else
        UISystem.KeyVerified = true
        -- Create open button if no key required
        UISystem:CreateOpenButton(config)
    end
    
    return UISystem
end

-- Setup drag functionality
function UISystem:SetupDrag(dragFrame)
    local MainFrame = self.Window.Main
    local dragging = false
    local dragStart, frameStart
    
    Creator.AddSignal(dragFrame.InputBegan, function(input)
        if input.UserInputType == Enum.UserInputType.MouseButton1 then
            dragging = true
            dragStart = input.Position
            frameStart = MainFrame.Position
            
            -- Bring to front
            local gui = MainFrame.Parent
            if gui then
                gui.ZIndexBehavior = Enum.ZIndexBehavior.Global
            end
        end
    end)
    
    Creator.AddSignal(dragFrame.InputEnded, function(input)
        if input.UserInputType == Enum.UserInputType.MouseButton1 then
            dragging = false
        end
    end)
    
    Creator.AddSignal(game:GetService("UserInputService").InputChanged, function(input)
        if dragging and input.UserInputType == Enum.UserInputType.MouseMovement then
            local delta = input.Position - dragStart
            MainFrame.Position = UDim2.new(
                frameStart.X.Scale,
                frameStart.X.Offset + delta.X,
                frameStart.Y.Scale,
                frameStart.Y.Offset + delta.Y
            )
        end
    end)
end

-- Setup events
function UISystem:SetupEvents()
    local MainFrame = self.Window.Main
    local CloseButton = MainFrame.TopBar.CloseButton
    local MinimizeButton = MainFrame.TopBar.MinimizeButton
    
    -- Close button
    Creator.AddSignal(CloseButton.MouseButton1Click, function()
        self:Toggle(false)
    end)
    
    Creator.AddSignal(CloseButton.MouseEnter, function()
        Tween(CloseButton, 0.2, {BackgroundColor3 = Color3.fromRGB(70, 70, 75)}):Play()
    end)
    
    Creator.AddSignal(CloseButton.MouseLeave, function()
        Tween(CloseButton, 0.2, {BackgroundColor3 = Color3.fromRGB(50, 50, 55)}):Play()
    end)
    
    -- Minimize button
    local isMinimized = false
    local originalSize = MainFrame.Size
    
    Creator.AddSignal(MinimizeButton.MouseButton1Click, function()
        if not isMinimized then
            -- Minimize
            Tween(MainFrame, 0.3, {
                Size = UDim2.new(originalSize.X.Scale, originalSize.X.Offset, 0, 80)
            }):Play()
            Tween(MainFrame.ContentArea, 0.3, {
                Visible = false
            }):Play()
            Tween(MainFrame.TabsContainer, 0.3, {
                Visible = false
            }):Play()
        else
            -- Restore
            Tween(MainFrame, 0.3, {
                Size = originalSize
            }):Play()
            Tween(MainFrame.ContentArea, 0.3, {
                Visible = true
            }):Play()
            Tween(MainFrame.TabsContainer, 0.3, {
                Visible = true
            }):Play()
        end
        isMinimized = not isMinimized
    end)
    
    Creator.AddSignal(MinimizeButton.MouseEnter, function()
        Tween(MinimizeButton, 0.2, {BackgroundColor3 = Color3.fromRGB(70, 70, 75)}):Play()
    end)
    
    Creator.AddSignal(MinimizeButton.MouseLeave, function()
        Tween(MinimizeButton, 0.2, {BackgroundColor3 = Color3.fromRGB(50, 50, 55)}):Play()
    end)
end

-- Check key system
function UISystem:CheckKeySystem(keyConfig)
    if not isfile(keyConfig.Filename .. ".key") then
        -- Show key system
        KeySystemModule.new({
            Window = self.Window,
            Title = keyConfig.Title or "Authentication Required",
            KeySystem = keyConfig,
            Folder = keyConfig.Folder or "Keys",
            Callback = function(success)
                if success then
                    self.KeyVerified = true
                    self:CreateOpenButton(keyConfig)
                    self:Toggle(true)
                else
                    warn("Key verification failed")
                end
            end
        })
    else
        -- Key exists, verify it
        local savedKey = readfile(keyConfig.Filename .. ".key")
        
        if keyConfig.KeyValidator then
            local isValid = keyConfig.KeyValidator(savedKey)
            if isValid then
                self.KeyVerified = true
                self:CreateOpenButton(keyConfig)
            else
                -- Invalid key, show key system again
                KeySystemModule.new({
                    Window = self.Window,
                    Title = keyConfig.Title or "Authentication Required",
                    KeySystem = keyConfig,
                    Folder = keyConfig.Folder or "Keys",
                    Callback = function(success)
                        if success then
                            self.KeyVerified = true
                            self:CreateOpenButton(keyConfig)
                            self:Toggle(true)
                        end
                    end
                })
            end
        else
            -- No validator, just accept the key
            self.KeyVerified = true
            self:CreateOpenButton(keyConfig)
        end
    end
end

-- Create open button
function UISystem:CreateOpenButton(config)
    if self.OpenButton then return end
    
    local OpenButton = OpenButtonModule.New({
        Window = {
            Title = config.Title or "UI System",
            Icon = config.Icon or "menu",
            Parent = self.Window.ScreenGui,
            Position = config.OpenButtonPosition or UDim2.new(0, 20, 0, 20)
        }
    })
    
    self.OpenButton = OpenButton
    
    -- Connect open button click
    Creator.AddSignal(OpenButton.Button.MouseButton1Click, function()
        self:Toggle()
    end)
end

-- Toggle UI visibility
function UISystem:Toggle(forceState)
    if self.RequiresKey and not self.KeyVerified then
        warn("Key verification required")
        return
    end
    
    local MainFrame = self.Window.Main
    local newState = forceState or not self.IsOpen
    
    if newState then
        -- Open
        MainFrame.Visible = true
        MainFrame.Position = UDim2.new(0.5, 0, 0.5, 0)
        MainFrame.BackgroundTransparency = 1
        
        Tween(MainFrame, 0.3, {
            BackgroundTransparency = 0.05
        }):Play()
        
        -- Scale animation
        MainFrame.Size = UDim2.new(0, 0, 0, 0)
        Tween(MainFrame, 0.4, {
            Size = self.Window.Config.Size
        }, Enum.EasingStyle.Back, Enum.EasingDirection.Out):Play()
    else
        -- Close
        Tween(MainFrame, 0.3, {
            BackgroundTransparency = 1,
            Size = UDim2.new(0, 0, 0, 0)
        }):Play()
        
        task.wait(0.3)
        MainFrame.Visible = false
    end
    
    self.IsOpen = newState
end

-- Add tab to UI
function UISystem:AddTab(tabConfig)
    if not self.KeyVerified then return nil end
    
    local tab = TabModule.New({
        Title = tabConfig.Title or "Tab",
        Icon = tabConfig.Icon,
        IconColor = tabConfig.IconColor,
        Content = tabConfig.Content,
        Parent = self.Window.TabsContainer,
        Border = tabConfig.Border or false,
        Locked = tabConfig.Locked or false,
        ShowTabTitle = tabConfig.ShowTabTitle or true,
        Desc = tabConfig.Desc
    })
    
    -- Store tab reference
    table.insert(self.Tabs, {
        Config = tabConfig,
        Instance = tab,
        Index = #self.Tabs + 1
    })
    
    -- Update tabs container size
    local totalWidth = 0
    for _, child in ipairs(self.Window.TabsContainer:GetChildren()) do
        if child:IsA("Frame") then
            totalWidth = totalWidth + child.AbsoluteSize.X + 8
        end
    end
    
    self.Window.TabsContainer.CanvasSize = UDim2.new(0, totalWidth, 0, 0)
    
    -- Select first tab if none selected
    if not self.CurrentTab then
        self:SelectTab(1)
    end
    
    return tab
end

-- Select tab
function UISystem:SelectTab(tabIndex)
    if tabIndex < 1 or tabIndex > #self.Tabs then return end
    
    -- Hide current tab content
    if self.CurrentTab then
        local currentTabInstance = self.Tabs[self.CurrentTab].Instance
        if currentTabInstance and currentTabInstance.ContainerFrame then
            currentTabInstance.ContainerFrame.Visible = false
        end
    end
    
    -- Show selected tab content
    local selectedTab = self.Tabs[tabIndex]
    if selectedTab and selectedTab.Instance and selectedTab.Instance.ContainerFrame then
        selectedTab.Instance.ContainerFrame.Visible = true
        
        -- Add to content area if not already
        if not selectedTab.Instance.ContainerFrame.Parent then
            selectedTab.Instance.ContainerFrame.Parent = self.Window.ContentArea
        end
    end
    
    self.CurrentTab = tabIndex
end

-- Add button to current tab
function UISystem:AddButton(config)
    if not self.CurrentTab then return nil end
    
    local currentTab = self.Tabs[self.CurrentTab]
    if not currentTab then return nil end
    
    local button = Button.New(
        config.Title or "Button",
        config.Icon,
        config.Callback,
        config.Variant or "Primary",
        currentTab.Instance.ContainerFrame,
        config.Dialog,
        config.FullRounded,
        config.Radius
    )
    
    return button
end

-- Add toggle to current tab
function UISystem:AddToggle(config)
    if not self.CurrentTab then return nil end
    
    local currentTab = self.Tabs[self.CurrentTab]
    if not currentTab then return nil end
    
    local toggleContainer, toggle = Toggle.New(
        config.Value or false,
        config.Icon,
        config.IconSize,
        currentTab.Instance.ContainerFrame,
        config.Callback,
        config.NewElement or false,
        {
            Window = self.Window
        }
    )
    
    return {
        Container = toggleContainer,
        Toggle = toggle
    }
end

-- Add input to current tab
function UISystem:AddInput(config)
    if not self.CurrentTab then return nil end
    
    local currentTab = self.Tabs[self.CurrentTab]
    if not currentTab then return nil end
    
    local input = Input.New(
        config.Placeholder or "Enter text...",
        config.Icon,
        currentTab.Instance.ContainerFrame,
        config.Type or "Input",
        config.Callback,
        config.OnChange or false,
        config.Radius,
        config.ClearTextOnFocus or false
    )
    
    return input
end

-- Add dropdown to current tab
function UISystem:AddDropdown(config)
    if not self.CurrentTab then return nil end
    
    local currentTab = self.Tabs[self.CurrentTab]
    if not currentTab then return nil end
    
    local dropdown = Dropdown.New({
        Title = config.Title or "Dropdown",
        Values = config.Values or {"Option 1", "Option 2", "Option 3"},
        Multi = config.Multi or false,
        Callback = config.Callback,
        MenuWidth = config.MenuWidth or 200,
        SearchBarEnabled = config.SearchBarEnabled or false,
        Parent = currentTab.Instance.ContainerFrame,
        Window = self.Window,
        WindUI = {
            DropdownGui = self.Window.ScreenGui
        }
    })
    
    return dropdown
end

-- Create notification
function UISystem:Notify(config)
    -- Simple notification implementation
    local notification = New("Frame", {
        Name = "Notification",
        Size = UDim2.new(0, 300, 0, 80),
        Position = UDim2.new(1, -320, 1, -100),
        AnchorPoint = Vector2.new(1, 1),
        BackgroundColor3 = Color3.fromRGB(35, 35, 40),
        Parent = self.Window.ScreenGui,
        ZIndex = 100
    }, {
        New("UICorner", {
            CornerRadius = UDim.new(0, 8)
        }),
        New("UIStroke", {
            Color = Color3.fromRGB(60, 60, 65),
            Thickness = 1
        }),
        New("TextLabel", {
            Name = "Title",
            Text = config.Title or "Notification",
            Size = UDim2.new(1, -60, 0, 30),
            Position = UDim2.new(0, 12, 0, 8),
            BackgroundTransparency = 1,
            TextColor3 = Color3.fromRGB(240, 240, 240),
            TextSize = 14,
            Font = Enum.Font.GothamSemibold,
            TextXAlignment = Enum.TextXAlignment.Left
        }),
        New("TextLabel", {
            Name = "Content",
            Text = config.Content or "",
            Size = UDim2.new(1, -60, 1, -40),
            Position = UDim2.new(0, 12, 0, 30),
            BackgroundTransparency = 1,
            TextColor3 = Color3.fromRGB(200, 200, 200),
            TextSize = 12,
            Font = Enum.Font.Gotham,
            TextXAlignment = Enum.TextXAlignment.Left,
            TextYAlignment = Enum.TextYAlignment.Top,
            TextWrapped = true
        }),
        New("ImageLabel", {
            Name = "Icon",
            Image = config.Image or "rbxassetid://7072725342",
            Size = UDim2.new(0, 24, 0, 24),
            Position = UDim2.new(1, -30, 0, 12),
            AnchorPoint = Vector2.new(1, 0),
            BackgroundTransparency = 1,
            ImageColor3 = Color3.fromRGB(200, 200, 200)
        })
    })
    
    -- Auto-remove after 5 seconds
    task.delay(5, function()
        if notification and notification.Parent then
            Tween(notification, 0.3, {
                Position = UDim2.new(1, -320, 1, 100)
            }):Play()
            task.wait(0.3)
            notification:Destroy()
        end
    end)
    
    return notification
end

-- Example usage
function UISystem:CreateExampleUI()
    -- Create main UI
    local ui = UISystem:Create({
        Title = "My UI System",
        Icon = "settings",
        Window = {
            Size = UDim2.new(0, 500, 0, 600),
            Position = UDim2.new(0.5, 0, 0.5, 0),
            Parent = game.Players.LocalPlayer:WaitForChild("PlayerGui")
        }
    })
    
    -- Add tabs
    local mainTab = ui:AddTab({
        Title = "Main",
        Icon = "home",
        IconColor = Color3.fromRGB(100, 150, 255)
    })
    
    local settingsTab = ui:AddTab({
        Title = "Settings",
        Icon = "settings",
        IconColor = Color3.fromRGB(255, 150, 100)
    })
    
    -- Add elements to main tab
    ui:SelectTab(1) -- Select main tab
    
    -- Add button
    ui:AddButton({
        Title = "Click Me",
        Icon = "mouse-pointer",
        Callback = function()
            print("Button clicked!")
            ui:Notify({
                Title = "Button Clicked",
                Content = "You clicked the button!",
                Image = "check-circle"
            })
        end
    })
    
    -- Add toggle
    local toggle = ui:AddToggle({
        Title = "Enable Feature",
        Value = false,
        Callback = function(state)
            print("Toggle state:", state)
        end
    })
    
    -- Add input
    ui:AddInput({
        Placeholder = "Enter your name...",
        Icon = "user",
        Callback = function(text)
            print("Input text:", text)
        end
    })
    
    -- Add dropdown
    ui:AddDropdown({
        Title = "Select Option",
        Values = {"Option 1", "Option 2", "Option 3"},
        Callback = function(selected)
            print("Selected:", selected)
        end
    })
    
    return ui
end

return UISystem
