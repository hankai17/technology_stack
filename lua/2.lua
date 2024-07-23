local alb_region = {
    BJFINRegion = "cn-beijing-1",
    BJGOVRegion = "cn-beijing-1",
    BJYZVPCRegion = "cn-beijing-1",
    BJZJMVPCRegion = "cn-beijing-1",
    CNNorthVIP1Region = "cn-beijing-1",
    GZVPCRegion = "cn-guangzhou-1",
    HKVPCRegion = "cn-guangzhou-1",
    NXRegionOne = "cn-ningxia-1",
    QYRegion = "cn-ningxia-1",
    QYRegionTwo = "cn-ningxia-1",
    RUSSRegionOne = "eu-east-1",
    SGPRegionOne = "sg-east-1",
    SHFINRegion = "cn-shanghai-1",
    SHPBSRegionOne = "cn-shanghai-1",
    TAIPEIVPCRegion = "cn-taibei-1",
    TJWQRegion = "cn-beijing-1",
    WHRegion = "cn-shanghai-1",
    alb = "alb"     -- default                                                                                        
}

local function transfer_region(region)
    local region = alb_region[region]
    if not region then
        return "alb"
    end
    return region
end

local res = transfer_region("WHRegion")
print(res)
--print(alb_region.is_true)

if alb_region.is_true == true then
    print("???")
else
    print("---")
end
