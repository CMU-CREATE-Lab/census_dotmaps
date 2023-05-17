-- TODO:
-- return all the jsonbs as records
-- test performance
-- modify python to cope with multiple records


--drop function if exists generate_points_x(geom geometry, block_idx bigint, dotcount integer);
create or replace function dotmap_generate_points(geom geometry, dotcount integer)
returns jsonb
language plpgsql
as $$
declare
    rec record;
    --film_title film.title%type;
begin
    while 1 > 0 loop
        -- Select first record into rec
        SELECT jsonb_agg(ST_X((dp).geom)) as x, jsonb_agg(ST_Y((dp).geom)) as y
        INTO rec
        FROM (
             -- Points from blocks
            SELECT 
                ST_DumpPoints(
                    ST_GeneratePoints(
                        -- Transform to Web Mercator 0-256, 0-256
                        ST_Affine(
                            ST_Transform(geom, 3857),
                            128/20037508.342789244, 0, 
                            0, -128/20037508.342789244,
                            128, 128),
                        ceil(dotcount*1.2)::int)) AS dp -- TODO: remove seed was ,1
                limit dotcount
        ) as points;
        if jsonb_array_length(rec.x) = dotcount then
            return to_jsonb(rec);
        end if;
    end loop;
end;$$;


--explain analyze
select indexes.idx as block_idx, dotmap_generate_points(geom, dotcounts.dotcount) as points
from tiger_wgs84.tl_2020_tabblock20 as blocks
join dotmaps.geoids_2020 as indexes on blocks.geoid20 = indexes.geoid
join dotmaps.prototile_2020_dotcount as dotcounts on indexes.idx = dotcounts.idx
where blocks.geoid20 between '010439653003024' and '010450201001021z';




