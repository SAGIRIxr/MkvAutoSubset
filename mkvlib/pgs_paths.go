package mkvlib

import "path"

const (
	pgsSubtitlePattern = `\.(pgs|sup)$`
	muxSubtitlePattern = `\.(sub|pgs|sup)$`
)

func pgsOutputPath(output, base string) string {
	return path.Join(output, base+".sup")
}
