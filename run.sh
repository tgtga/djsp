ruby -r./djsp -e 'DJSP::C.setup; DJSP::C.sequence 2, 1, 10000 ** 2, nil, nil, ->(index, mark, where){ DJSP.message "A(#{index}) @ #{where} = #{mark}" }'
