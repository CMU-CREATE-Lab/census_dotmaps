-- 3 seconds for 6K blocks, 660K points

explain analyze

-- Return json-aggregated columns
SELECT json_agg(block_idx) as block_idx, json_agg((dp).path[1]-1) as dot_idx, json_agg(ST_X((dp).geom)) as x, json_agg(ST_Y((dp).geom)) as y
FROM (
    -- Points from blocks
    SELECT 
        block_idx, 
        ST_DumpPoints(
            ST_GeneratePoints(
                -- Transform to Web Mercator 0-256, 0-256
                ST_Affine(
                    ST_Transform(geom, 3857),
                    128/-20037508.342789244, 0, 
                    0, 128/-20037508.342789244,
                    128, 128),
                -- Number of points: TODO 
                100)) AS dp
    FROM (
        -- index and geom for blocks in county
        select idx as block_idx, geom
        from tiger_wgs84.tl_2020_tabblock20 as blocks
        join dotmaps.geoids_2020 as indexes on blocks.geoid20 = indexes.geoid
        -- Select single county
        where blocks.geoid20 between '42125' and '42125z'
    ) AS blocks
) as points
