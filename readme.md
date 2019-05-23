# turret_lib

This library, **turret_lib**, provides a turretClient and turretLogger class, which communicate with a **turret-server**, and can be included in any c++ plugin which needs to resolve shotgun tank uris.  

UTSALA provides two example projects which use **turret_lib**: turret_usd and turref_klf (katana).  

### Examples

An example of a successfully resolved shotgun tank URI would be:
URI:`tank:/s118/maya_publish_asset_cache_usd?Step=model&Task=model&asset_type=setPiece&version=latest&Asset=building01`
Path:`/mnt/ala/mav/2018/jobs/s118/assets/setPiece/building01/model/model/caches/usd/building01_model_model_usd.v045.usd`

#### Environment Variables

Turret allows some basic settings to be overriden via environment variables

 * TURRET\_SESSION\_ID
 * TURRET\_SERVER\_IP
 * TURRET\_SERVER\_PORT
 * TURRET\_TIMEOUT
 * TURRET\_RETRIES
 * DEBUG\_LOG\_LEVEL
 * DEBUG\_ENABLED
