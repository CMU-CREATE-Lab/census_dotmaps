-- 3 seconds for 6K blocks, 660K points

explain analyze

-- Return json-aggregated columns
SELECT json_agg(geoid20) as geoid20, json_agg((dp).path[1]-1) as dotidx, json_agg(ST_X((dp).geom)) as x, json_agg(ST_Y((dp).geom)) as y
FROM (
    -- Points from blocks
    SELECT 
        geoid20, 
        ST_DumpPoints(
            ST_GeneratePoints(
                -- Transform to Web Mercator 0-256, 0-256
                ST_Affine(
                    ST_Transform(geom, 3857),
                    128/-20037508.342789244, 0, 
                    0, 128/-20037508.342789244,
                    128, 128),
                -- Number of points
                100)) AS dp
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
