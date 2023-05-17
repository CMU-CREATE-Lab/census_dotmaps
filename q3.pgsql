explain analyze

-- Return json-aggregated columns
SELECT json_agg(geoid20) as geoid20, json_agg((dp).path[1]-1) as dotidx, json_agg(ST_X((dp).geom)) as x, json_agg(ST_Y((dp).geom)) as y
FROM (
    -- Points from blocks
    SELECT geoid20, ST_DumpPoints(ST_GeneratePoints(geom,100)) AS dp
    FROM (
        -- Blocks from county
        select * 
        from tiger_wgs84.tl_2020_tabblock20 
        where geoid20 between '42125' and '42125z'
        --limit 2
        -- TODO: join with geoid index
        -- TODO: preject to web mercator
    ) AS blocks
) as points
